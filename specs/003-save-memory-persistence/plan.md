# Plan 003

1. Analizar estado volátil, rangos y reglas upstream; escribir especificación.
2. Implementar almacenamiento acotado con reemplazo atómico y respaldo.
3. Integrar Setup/Get/Set/Sync y ciclo de vida con mutex/errores guest.
4. Añadir regresiones reales, incluyendo procesos separados y archivos heredados.
5. Registrar suite en CMake/CI y sincronizar versión 0.4.0/changelog.
6. Revisar almacenamiento, límites y diff con agentes; validar formato y YAML.
7. Publicar rama, compilar/probar tres plataformas y ejecutar CodeQL.
8. Integrar tras CI satisfactoria, publicar paquetes oficiales y cerrar SDD.

## Evidencia de rama — 2026-10-13

- Implementación `79cdaf74ca58d94b6139a2918547a998562f71df`.
- [Build 38417110811](https://github.com/erickson558/KatyPS5/actions/runs/38417110811):
  Windows 9/9, Linux 11/11, macOS 9/9; total 29 CTest, incluidos los procesos
  separados de escritura/lectura y limpieza.
- [CodeQL 38417117484](https://github.com/erickson558/KatyPS5/actions/runs/38417117484): success.
- Agentes `save_storage`, `save_tests`, `save_review`: almacenamiento, regresiones
  reales y revisión de límites/archivos/CMake/CI, sin hallazgos pendientes.
- Se habilitan excepciones solo en los tres TUs de guardado/prueba para convertir
  errores de asignación/filesystem en estados guest; el resto del proyecto conserva
  sus opciones. Formato, YAML, bloques CMake, versión y diff revisados localmente.
- Después de esta CI solo se añade la exclusión `_SaveData/` a Git y su nota
  documental; fuentes, tests, CMake y workflows permanecen idénticos al código
  validado. La compilación oficial de main verificará el HEAD integrado completo.

Publicación autorizada por el propietario. Skills locales aplicados:
`debug-fix-release`, `code-annotator` y `github-publish`; revisión de archivos y
memoria según `security-auditor`. Diff para revisión en el artefacto ignorado
`_Build/review/katyps5-0.4.0.patch`; logs locales bajo `_Build/review/ci-38417110811`.

Comandos: `git switch -c feat/save-memory-persistence-0.4.0` aisló el cambio;
`git add` por rutas y `git commit` lo registraron; `git push -u origin` publicó
la rama; `gh workflow run build.yml/codeql.yml --ref ...` ejecutó la verificación
sin crear una release de rama. Pendiente integración y entrega oficial.
