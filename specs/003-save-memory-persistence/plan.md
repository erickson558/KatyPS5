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
sin crear una release de rama.

## Entrega oficial — 2026-10-13

Integración por fast-forward en main `fff02fc596b7c0e3541d6b741eb89faf32847de6`.
`git push origin main` inició la compilación oficial:

- [Build 38430039774](https://github.com/erickson558/KatyPS5/actions/runs/38430039774):
  success, Windows 9/9, Linux 11/11, macOS 9/9, total 29 CTest. Install y
  artefactos satisfactorios. Logs/reporte local en `_Build/review/ci-38430039774`.
- [CodeQL 38430039830](https://github.com/erickson558/KatyPS5/actions/runs/38430039830):
  success. Revisión adicional del agente no encontró alertas abiertas para los
  dos archivos de guardados; esto no equivale a una auditoría completa del emulador.
- Release fechada `KatyPS5-2026-10-13-fff02fc` con target exacto del build.
  Se descargaron sus tres paquetes y se verificaron hashes y avisos LICENSE /
  LICENSES/Kyty-MIT.txt contra el código (normalizando fin de línea).
- El ejecutable Windows local abrió --help con salida 0 y reportó 0.4.0 / fff02fc.
  Launcher: `_Build/releases/v0.4.0/windows/launcher.exe`.
- Tag anotado `v0.4.0` en el commit oficial; [release semver publicada](https://github.com/erickson558/KatyPS5/releases/tag/v0.4.0)
  y marcada latest con los mismos bytes de los paquetes oficiales. Manifiesto
  local: `_Build/releases/v0.4.0/manifest.json`.

| Paquete | SHA256 |
| --- | --- |
| Windows-x64.zip | 8b32c2280da2117435a7e67c4ed0a1effd08b6550d1d9017ee990391c9509663 |
| macOS-x86_64.zip | 9b839f9f79425babe2fe23e75b593377a9f5a1d9b7eb5a2fdb6ebbf1dbf42d76 |
| Linux-x86_64.tar.gz | a800d05f412fce8a92f3416af885fe1ab7e85a8a6a2ebe75d1f7444532dc8815 |

Comandos finales: `git merge --ff-only` integró la rama; `git push origin main`
publicó la integración; `git tag -a v0.4.0 <SHA>` y `git push origin v0.4.0`
publicaron el tag; `gh release create --verify-tag --draft --notes-file ...`
subió los tres paquetes comprobados; `gh release edit --draft=false --latest`
publicó la release después del CodeQL oficial satisfactorio. Se corrigió la
selección de credenciales con `gh auth switch --user erickson558` y
`gh auth setup-git` tras un push rechazado con la otra cuenta autenticada.

No se probaron juegos reales ni FPS. El tag y los binarios son inmutables; este
cierre posterior solo cambia documentación y no requiere reconstrucción.
