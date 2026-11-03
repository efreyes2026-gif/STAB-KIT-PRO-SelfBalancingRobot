% =========================================================================
% CONTROLADOR DIFUSO MAMDANI SINTONIZADO EN VOLTIOS DIRECTOS [-6V, +6V]
% ENTRADA 1: Error angular [rad]            [-0.40, +0.40]
% ENTRADA 2: Velocidad angular [rad/s]      [-4.00, +4.00]
% SALIDA:    Tensión de control [V]         [-6.00, +6.00]
% =========================================================================
clc;
fprintf('\n=========================================================\n');
fprintf('     CREACIÓN DEL CONTROLADOR DIFUSO (MAMDANI VOLTIOS)\n');
fprintf('=========================================================\n');

%% 1. CREACIÓN DEL SISTEMA MAMDANI
fis = mamfis('Name', 'controlador_difuso_robot');
fis.AndMethod = 'min';
fis.OrMethod = 'max';
fis.ImplicationMethod = 'min';
fis.AggregationMethod = 'max';
fis.DefuzzificationMethod = 'centroid';

%% 2. UNIVERSOS DE DISCURSO ADAPTADOS A SIMSCAPE
% Entrada 1: Error angular e = 0 - theta [rad]
fis = addInput(fis, [-0.40 0.40], 'Name', 'error_angular');

% Entrada 2: Velocidad angular theta_dot directa de Simscape [rad/s]
fis = addInput(fis, [-4.00 4.00], 'Name', 'velocidad_angular');

% Salida: Tensión directa a los motores [V]
fis = addOutput(fis, [-6.00 6.00], 'Name', 'u_volts');

%% 3. FUNCIONES DE PERTENENCIA (5 PARTICIONES TRIANGULARES)
etiquetas = {'NB', 'NS', 'ZE', 'PS', 'PB'};

% --- Entrada 1: Error angular (rad) ---
mf_error = {
    [-0.40 -0.40 -0.20]   % NB: Negativo Grande
    [-0.40 -0.20  0.00]   % NS: Negativo Pequeño
    [-0.20  0.00  0.20]   % ZE: Cero
    [ 0.00  0.20  0.40]   % PS: Positivo Pequeño
    [ 0.20  0.40  0.40]   % PB: Positivo Grande
};
for k = 1:5
    fis = addMF(fis, 'error_angular', 'trimf', mf_error{k}, 'Name', etiquetas{k});
end

% --- Entrada 2: Velocidad angular (rad/s) ---
mf_vel = {
    [-4.00 -4.00 -2.00]   % NB
    [-4.00 -2.00  0.00]   % NS
    [-2.00  0.00  2.00]   % ZE
    [ 0.00  2.00  4.00]   % PS
    [ 2.00  4.00  4.00]   % PB
};
for k = 1:5
    fis = addMF(fis, 'velocidad_angular', 'trimf', mf_vel{k}, 'Name', etiquetas{k});
end

% --- Salida: Voltaje hacia los motores (V) ---
mf_salida = {
    [-6.00 -6.00 -3.00]   % NB (-6V)
    [-6.00 -3.00  0.00]   % NS (-3V)
    [-3.00  0.00  3.00]   % ZE (0V)
    [ 0.00  3.00  6.00]   % PS (+3V)
    [ 3.00  6.00  6.00]   % PB (+6V)
};
for k = 1:5
    fis = addMF(fis, 'u_volts', 'trimf', mf_salida{k}, 'Name', etiquetas{k});
end

%% 4. BASE DE 25 REGLAS (ACCIÓN PROPORCIONAL + AMORTIGUAMIENTO DERIVATIVO)
% Convención física:
% Error positivo (robot inclinado hacia atrás) -> Demanda par positivo (+V)
% Velocidad angular positiva (robot cayendo hacia adelante) -> Demanda freno (-V)
reglas = zeros(25, 5);
n = 0;
for i = 1:5       % Índice de error
    for j = 1:5   % Índice de velocidad
        n = n + 1;
        % Matriz diagonal balanceada tipo PD difuso
        indice_salida = min(5, max(1, i - j + 3));
        reglas(n, :) = [i, j, indice_salida, 1, 1];
    end
end
fis = addRule(fis, reglas);

%% 5. GUARDAR Y ASIGNAR AL WORKSPACE DE SIMULINK
carpetaDestino = 'C:\Users\efrey\OneDrive\Documentos\MATLAB';
if ~isfolder(carpetaDestino), carpetaDestino = pwd; end
archivoFIS = fullfile(carpetaDestino, 'controlador_difuso_robot.fis');

writeFIS(fis, archivoFIS);

% Cargar en el Workspace bajo ambos nombres para evitar incompatibilidad
assignin('base', 'controlador_difuso_robot', fis);
assignin('base', 'fis', fis);

fprintf('\nControlador difuso generado y guardado en: %s\n', archivoFIS);
fprintf('Total de reglas asignadas: %d\n', numel(fis.Rules));

%% 6. PRUEBA DE RESPUESTA EN CONDICIÓN CRÍTICA
v_rescate = evalfis(fis, [0.35, 0]); % Inclinación inicial de -20° sin velocidad
v_origen  = evalfis(fis, [0.00, 0]); % Vertical en equilibrio

fprintf('\n--- VALIDACIÓN DE CONTROL DIFUSO ---');
fprintf('\n  Tensión ante caída inicial (+0.35 rad): %+0.2f V (Esperado: +5 a +6 V)', v_rescate);
fprintf('\n  Tensión en equilibrio (0.00 rad):       %+0.2f V (Esperado: 0.0 V)', v_origen);
fprintf('\n------------------------------------\n');