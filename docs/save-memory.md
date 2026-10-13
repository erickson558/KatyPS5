# Guardados de memoria — v0.4.0

SaveDataMemory2 conserva un snapshot completo de los datos y sus metadatos en:

```text
_SaveData/<TITLE_ID>/sce_sdmemory<slot>/<user_id>/memory.bin
```

El slot 0 usa `sce_sdmemory`, sin sufijo; slots 1–3 usan su número.
La raíz es relativa al directorio de trabajo del emulador, igual que los
guardados existentes. `_SaveData/` está excluida de Git para mantener el progreso
local fuera del repositorio. Cada llamada Set satisfactoria ya está escrita: no
depende de cerrar el juego correctamente ni de una llamada posterior a Sync.
Setup puede cargar un snapshot y ampliar su memoria con ceros; una petición
más pequeña se rechaza para evitar perder su cola en la siguiente escritura.

El snapshot contiene cabecera versionada, datos y SaveDataParam. Un checksum
detecta alteraciones accidentales; no proporciona autenticación criptográfica.
La escritura usa un temporal en el mismo directorio y lo reemplaza atómicamente.
`memory.bin.bak` conserva el snapshot anterior. Si una escritura falla, Set
reporta el error y conserva el estado de memoria anterior. No se promete
durabilidad frente a pérdida de alimentación ni escritura concurrente del mismo
slot desde dos procesos del emulador.

## Archivos anteriores

Cuando no existe `memory.bin`, se puede importar `memory.dat` y, si existe,
`param.bin` del mismo directorio de usuario/slot. Los originales se conservan;
el nuevo snapshot se crea al escribir o sincronizar. Los guardados clásicos
montados mantienen sus rutas y formato. v0.3.2 y anteriores no persistían
SaveDataMemory2: el progreso de ese vector ya perdido al cerrar no es recuperable.

## Recuperación explícita

Si el snapshot está corrupto, el emulador devuelve `SAVE_DATA_ERROR_BROKEN` y
conserva los archivos. Con el emulador cerrado, conserva una copia del
`memory.bin` dañado y copia `memory.bin.bak` como `memory.bin`. La siguiente
carga recuperará el último snapshot anterior si el respaldo es válido.

## Límites y validación

Hay cuatro estados activos en total y un presupuesto compartido de 32 MiB de
datos; la opción de doble buffer cuenta dos veces. Los metadatos tienen un
tamaño fijo adicional pequeño. Get/Set solo acceden a la memoria preparada:
no la amplían. Set valida hasta cinco descriptores antes de aplicar todo el
lote y mantiene la convención de un descriptor cuando `data_num=0` y `data`
es no nulo. Los iconos se aceptan pero todavía no se persisten.

## Verificación

La suite `save_data_memory` comprueba aislamiento, metadatos, límites,
desbordamientos, importación, errores de I/O y recuperación. Las fixtures
`save_data_memory_write`, `save_data_memory_restart` y
`save_data_memory_cleanup` verifican un reinicio real en procesos separados.
Las pruebas usan títulos y archivos temporales; no prueban juegos comerciales.
