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
- [x] Compilar y ejecutar pruebas en GitHub Actions: Windows 5/5, Linux 7/7, macOS 5/5.
- [x] Corregir el fallo observado de invalidación antes del renderer y repetir CI.
- [x] CodeQL del commit corregido satisfactorio (38324030155).
- [x] Integrar por fast-forward en main, compilar oficialmente y publicar v0.3.2 tras CI satisfactoria.

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

La compilación y CTest se ejecutaron en GitHub Actions al faltar el toolchain local.
El formato se comprobó con una herramienta aislada en `_Build/format-tools`, ignorada por Git.
El usuario autorizó commit/push y compilación en CI el 2026-10-12. La validación
de publicación se actualizará con commits y enlaces de ejecuciones reales.

## Validación ejecutable

- Implementación: `34c57412`; primera ejecución
  [38322431144](https://github.com/erickson558/KatyPS5/actions/runs/38322431144).
  Las tres plataformas compilaron; la prueba FS detectó invalidación sin gestor GPU.
- Corrección: `b8eb8d59`;
  [38324027422](https://github.com/erickson558/KatyPS5/actions/runs/38324027422)
  terminó satisfactoriamente: Windows 5/5, Linux 7/7 y macOS 5/5 suites, install y
  artefactos verificados. CodeQL del primer commit también terminó correctamente;
  [38324030155](https://github.com/erickson558/KatyPS5/actions/runs/38324030155)
  terminó correctamente para el commit corregido.
- `64bdedcd` solo cambia triggers de CI/documentación; agente de CI verificó las
  exclusiones documentales. El código ejecutable sigue siendo el de `b8eb8d59`.
- Agentes: revisión de código/audio, CI/plataformas y publicación/licencias.
- Skills utilizadas: `debug-fix-release`, `github-publish`, `code-annotator` y
  revisión de las instrucciones de `security-hardening` para CodeQL existente.

Windows local: el paquete CI de `b8eb8d59` ejecutó `katy_emulator --help` con
salida 0 y versión 0.3.2. LICENSE y LICENSES/Kyty-MIT.txt coinciden por SHA256
con los originales. Preview local: `_Build/preview-b8eb8d59/windows`.

## Publicación final — 2026-10-13

- Rama integrada por fast-forward; `main` sigue `origin/main` en vez de upstream.
- Código publicado: `bc5211aac2fe2d81a4abe777488b464f080f1576`.
- [Build/Release oficial 38391264080](https://github.com/erickson558/KatyPS5/actions/runs/38391264080):
  éxito, Windows 5/5, Linux 7/7, macOS 5/5 e instalación/artefactos correctos.
- [CodeQL main 38391264142](https://github.com/erickson558/KatyPS5/actions/runs/38391264142): éxito.
- Tag anotado y [release v0.3.2](https://github.com/erickson558/KatyPS5/releases/tag/v0.3.2):
  tres paquetes oficiales, hashes SHA256 iguales a los assets de la ejecución
  exacta de main y avisos LICENSE/LICENSES verificados en cada plataforma.
- Binario oficial Windows descargado y ejecutado en este equipo con `--help`:
  salida 0, versión 0.3.2, git bc5211a. Ubicación: `_Build/releases/v0.3.2/windows`.
- Notas y manifest SHA256 locales: `_Build/releases/v0.3.2`.
- Publicación realizada con cuenta `erickson558`; actor de ejecución confirmado.
- SDD cerrado; sin cambios del formato de guardados/configuración ni nuevas dependencias.
  No se midieron FPS ni se probaron juegos reales.
