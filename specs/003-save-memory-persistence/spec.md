# Spec 003: Guardados de memoria persistentes y acotados

## Status
Done

## Context
El propietario aprobó implementar persistencia y validación de guardados, con
SDD, agentes, compilación y publicación. SaveDataMemory2 usa un vector global
volátil de 64 KiB; user_id/slot_id solo se imprimen. Get/Set amplían el vector
con offset + buf_size sin comprobar desbordamiento y un lote inválido puede
modificar parcialmente la memoria. Sync devuelve éxito sin escribir.

## Goal
- Conservar datos y metadatos tras reiniciar el proceso, separados por título,
  usuario y los cuatro slots de memoria.
- Rechazar rangos inválidos, tamaños excesivos y lotes parcialmente inválidos
  sin modificar la memoria ni el archivo anterior.
- Confirmar una actualización solo después de completar la escritura; mantener
  un respaldo recuperable del estado anterior.

## Non-goals
Carga de módulos, sincronización del kernel, iconos persistentes, cambios de
rutas de guardados clásicos montados y pruebas de compatibilidad con juegos.

## Approach
Adaptar las reglas de Setup/Get/Set/Sync de upstream cfbaee07 a la base Katy.
Usar estados por ruta _SaveData/TITLE/sce_sdmemory[SLOT]/USER, con mutex, cuatro
estados activos y presupuesto de 32 MiB (incluido doble buffer).
Los metadatos fijos son adicionales a ese presupuesto de datos; las copias
transitorias de carga/escritura también están acotadas por el tamaño máximo.
Conservar la
convención data_num=0 con data no nulo como un descriptor, y aceptar iconos sin
persistirlos. Setup carga sin escribir y reporta el tamaño anterior; Get no
amplía memoria; Set valida todo el lote antes de copiar y publicar.

Datos y SaveDataParam se guardan juntos en un snapshot versionado y acotado,
con comprobación de integridad, archivo temporal y reemplazo atómico del
destino; respaldo previo antes de reemplazar. Importar memory.dat/param.bin de
la misma ruta cuando no exista snapshot, sin modificar los originales. No
recuperar automáticamente archivos corruptos ni sobrescribirlos al cargar:
devolver BROKEN y conservar respaldo para recuperación explícita. Las rutas
clásicas del fork permanecen accesibles sin migración; versiones anteriores
nunca persistieron este vector, por lo que no existe un archivo propio perdido
que migrar. Validar el título usado para nuevos paths.

## Risk / impact
Código compartido Windows/Linux/macOS. Riesgos: requisitos más estrictos de
Setup/rangos, I/O síncrono y reemplazo del snapshot. Pruebas deben ejercitar
implementación real: reinicio en procesos distintos, aislamiento, metadatos,
lotes inválidos, overflow, límites, doble buffer, importación, respaldo,
archivo corrupto y fallo de escritura. Los checks existentes continúan en CI.
La atomicidad protege interrupciones del proceso; no se promete durabilidad
frente a pérdida de alimentación ni coordinación entre dos emuladores que
escriban simultáneamente el mismo slot. No se instala toolchain local.

## Version impact
Minor: 0.3.2 -> 0.4.0. Añade persistencia sin cambiar CLI/configuración ni
formatos de guardados clásicos. Compilación y CodeQL de rama antes de integrar;
artefactos oficiales de main antes de publicar v0.4.0.

## Resultado — 2026-10-13

Integrado en main `fff02fc596b7c0e3541d6b741eb89faf32847de6`, compilación
oficial y CodeQL satisfactorios. Windows 9/9, Linux 11/11 y macOS 9/9 CTest.
[v0.4.0 publicada](https://github.com/erickson558/KatyPS5/releases/tag/v0.4.0)
con tag anotado en ese SHA y los tres paquetes oficiales, hashes/licencias
verificados. Windows ejecutó --help localmente con salida 0 y versión/git
correctos. No se probaron juegos ni se midieron FPS. El cierre documental
posterior no modifica fuentes ni binarios. Evidencia completa en plan.md.
