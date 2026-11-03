% =========================================================================
% ENTRENAMIENTO DE RED NEURONAL MLP 2-8-1 (ADAPTADA A RADIANES Y SIMSCAPE)
% ENTRADAS: Error angular (rad) y Derivada (rad/s)
% SALIDA:   Esfuerzo de control PWM [-255, 255]
% =========================================================================
clc;
fprintf('\n=========================================================\n');
fprintf('     ENTRENAMIENTO RNA MLP 2-8-1 (SINTONIZADA PARA SIMSCAPE)\n');
fprintf('=========================================================\n');

%% 1. RUTAS Y ARCHIVOS
carpeta = 'C:\Users\efrey\OneDrive\Documentos\MATLAB';
if ~isfolder(carpeta)
    carpeta = pwd;
end
archivoFIS = fullfile(carpeta, 'controlador_difuso_robot.fis');
archivoRNA = fullfile(carpeta, 'modelo_rna_robot.mat');

%% 2. DEFINIR LA MALLA EN RADIANES (LO QUE ENTREGA SIMSCAPE)
% Rango físico real del péndulo:
% Error:     -0.35 rad a +0.35 rad  (equivalente a -20° a +20°)
% Derivada:  -2.10 rad/s a +2.10 rad/s (equivalente a -120°/s a +120°/s)
e_rad_vec  = linspace(-0.35, 0.35, 45);     % 45 puntos
de_rad_vec = linspace(-2.10, 2.10, 45);     % 45 puntos

[E_rad, DE_rad] = meshgrid(e_rad_vec, de_rad_vec);

% Matriz de entradas para la Red Neuronal (2 x N en RADIANES)
entradas = [E_rad(:)'; DE_rad(:)'];

%% 3. GENERAR EL OBJETIVO DE SALIDA EN ESCALA PWM [-255, 255]
if isfile(archivoFIS)
    fprintf('Cargando superficie del archivo FIS: %s\n', archivoFIS);
    fis = readfis(archivoFIS);

    % Se convierten temporalmente los radianes a grados SOLO para consultar el FIS
    E_deg  = E_rad * (180/pi);
    DE_deg = DE_rad * (180/pi);
    muestras_deg = [E_deg(:), DE_deg(:)];

    salidas_eval = evalfis(fis, muestras_deg);
    salidas_pwm  = reshape(salidas_eval, 1, []);
else
    % Si no encuentra el .fis, genera la superficie a partir del control estabilizador
    fprintf('Archivo FIS no encontrado en ruta. Generando superficie basada en PID estable...\n');
    Kp_pwm = 255 / 0.35;   % ~728.5 (255 PWM al llegar a 0.35 rad)
    Kd_pwm = 255 / 3.50;   % ~72.8 (amortiguamiento dinámico)

    u_teorico = Kp_pwm * E_rad(:)' + Kd_pwm * DE_rad(:)';
    salidas_pwm = max(min(u_teorico, 255), -255);
end

% Saturación estricta de seguridad en el rango del actuador
salidas_pwm = max(min(salidas_pwm, 255), -255);

fprintf('Muestras generadas: %d puntos\n', size(entradas, 2));
fprintf('Rango objetivo de PWM: [%.1f, %.1f]\n', min(salidas_pwm), max(salidas_pwm));

%% 4. CONFIGURAR LA ARQUITECTURA MLP 2-8-1
fprintf('\nConfigurando Perceptrón Multicapa 2-8-1...\n');
net = feedforwardnet(8, 'trainlm');

% Capa oculta: tangente hiperbólica sigmoidal (tansig)
net.layers{1}.transferFcn = 'tansig';
% Capa de salida: lineal (purelin)
net.layers{2}.transferFcn = 'purelin';

% División de muestras (70% entrenamiento, 15% validación, 15% prueba)
net.divideParam.trainRatio = 0.70;
net.divideParam.valRatio   = 0.15;
net.divideParam.testRatio  = 0.15;

% Criterios de optimización Levenberg-Marquardt
net.trainParam.epochs     = 500;
net.trainParam.goal       = 1e-4;
net.trainParam.min_grad   = 1e-6;
net.trainParam.max_fail   = 10;
net.trainParam.showWindow = true;

%% 5. ENTRENAR LA RED
fprintf('Iniciando entrenamiento supervisado...\n');
[net, tr] = train(net, entradas, salidas_pwm);
fprintf('¡Entrenamiento finalizado con éxito!\n');

%% 6. EVALUAR Y GUARDAR
salidas_red = net(entradas);
error_aprox = salidas_pwm - salidas_red;
rmse_pwm    = sqrt(mean(error_aprox.^2));

fprintf('\nError cuadrático medio (RMSE): %.4f unidades PWM\n', rmse_pwm);

% Guardar archivo .mat
save(archivoRNA, 'net', 'tr', 'entradas', 'salidas_pwm', 'rmse_pwm');
fprintf('Red guardada en: %s\n', archivoRNA);

% CARGAR AUTOMÁTICAMENTE EN EL WORKSPACE DE SIMULINK
assignin('base', 'net', net);
fprintf('\n>>> Variable "net" cargada en el Workspace base <<<\n');