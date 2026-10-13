# Plan 002

1. Comparar la historia y dependencias con upstream y seleccionar fixes aislados.
2. Documentar causa, impacto y riesgo antes de editar código.
3. Adaptar seis correcciones a los nombres Katy y la base SDL2.
4. Adaptar pruebas upstream de eventos/audio y añadir regresiones de filesystem.
5. Incluir pruebas en `katy_tests` y el workflow de las tres plataformas.
6. Sincronizar versión/changelog y documentar próximos backports por dependencia.
7. Revisar diff, sintaxis y referencias locales; presentar cambios al propietario.
8. Después de revisión humana, publicar la rama y verificar CI antes de integrar.

## Resultado

Los pasos se completaron. La primera CI detectó invalidación de GPU antes del
renderer; se analizó/documentó la causa, se corrigió y se repitió la matriz.
Rama y main aprobaron 17 suites y CodeQL. Release v0.3.2 publicada con paquetes
de main bc5211aa y verificación local del ejecutable Windows. Los informes de
cierre se publican como documentación; no cambian el código ya validado/tagueado.
