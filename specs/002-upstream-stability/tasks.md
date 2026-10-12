# Tasks 002

- [x] Comparar fork/upstream y comprobar el entorno disponible.
- [x] Acordar prioridad: compatibilidad y estabilidad.
- [x] Documentar causas, impacto y riesgos.
- [x] Adaptar las seis correcciones seleccionadas.
- [x] Añadir/adaptar regresiones y cobertura de CI.
- [x] Actualizar versión, changelog y análisis de mejoras restantes.
- [x] Validar estáticamente y revisar el diff final.
- [x] Revisión humana/autorización para publicar y corregir hallazgos de agentes/CI (2026-10-12).
- [x] Revisar con agentes y cubrir el reloj de audio cuando falta PCM.
- [ ] Compilar y ejecutar pruebas en GitHub Actions.
- [ ] Integrar y etiquetar solamente después de CI satisfactoria.

## Validación local realizada

- `git diff --check`: correcto.
- clang-format 22.1.3: rangos C++ modificados y nuevo test correctos.
- YAML de todos los workflows: parseo correcto.
- CMake: bloques equilibrados, headers locales existentes, tests registrados,
  dependencias de `katy_tests` y targets/filtros CI de tres plataformas coherentes.
- CMake/changelog: versión candidata 0.3.2 coincidente.
- Traducción española: XML válido y sin modificaciones.
- Submódulos, dependencias y licencias: sin modificaciones.
- Endpoint de compatibilidad: HTTP 200, esquema/estados compatibles, 325 entradas
  y 84,730 bytes; endpoint anterior HTTP 404.

No se ha compilado ni ejecutado CTest: falta el toolchain local. El formato se
comprobó con una herramienta aislada en `_Build/format-tools`, ignorada por Git.
El usuario autorizó commit/push y compilación en CI el 2026-10-12. La validación
de publicación se actualizará con commits y enlaces de ejecuciones reales.
