# Comparación KytyPS5 / KatyPS5 — 2026-10-12

Prioridad acordada: **compatibilidad y estabilidad**.

## Estado de la comparación

- Fork local: `erickson558/KatyPS5`, base revisada `aa9733a4` (0.3.1).
- Upstream: [KytyPS5/KytyPS5](https://github.com/KytyPS5/KytyPS5),
  snapshot `df5b9b92be7dfd463b47756b707d6d356a9f0550`.
- Ancestro común: `0bbd0bdd9928d6b96c08cd3fdb221ad24bd6de84`.
- Upstream tiene **1,167 commits** después de ese ancestro. Su diff contra la
  base común abarca **534 archivos**, 158,651 líneas añadidas y 83,304 eliminadas.
  Esas cifras describen el tamaño del cambio; no equivalen a 1,167 mejoras independientes.
- El fork conserva SDL2; upstream usa SDL3. También cambió la arquitectura del
  recompilador, cachés Vulkan, audio y launcher. Copiar archivos recientes sin
  seguir sus dependencias puede romper compilación, guardados o comportamiento.

## Correcciones adaptadas a la rama `fix/upstream-stability-0.3.2`

| Área | Causa comprobada en el fork | Resultado del cambio | Fuente |
| --- | --- | --- | --- |
| Archivos | `KernelLseek` retorna `EINVAL` para offset negativo con el mutex tomado. | RAII libera el mutex incluso en retornos de error. | [b9aa0dc3](https://github.com/KytyPS5/KytyPS5/commit/b9aa0dc3b397e3a5e7b59bf77bd6a551e07a8c75) |
| Descriptores | `KernelOpen` reserva un descriptor antes de rechazar `O_RDONLY \| O_TRUNC` y no lo libera. | Los intentos rechazados no consumen entradas ni objetos. | [0f3674f5](https://github.com/KytyPS5/KytyPS5/commit/0f3674f5319ab6736bae406f6bc47435f06548f7) |
| Imports | Existe `KernelFtruncate`, pero falta su NID nativo en `InitLibKernel_1_FS`. | `sceKernelFtruncate` resuelve a la implementación existente. | [048fe9f4](https://github.com/KytyPS5/KytyPS5/commit/048fe9f4134392fa6652fe2d3c32f718eb4d2977) |
| Eventos | Un evento de nivel sigue marcado como disparado y se repite hasta llenar el lote. | Se entrega una vez por espera y deja avanzar los siguientes eventos; los payloads pendientes mantienen orden. | [afbb322b](https://github.com/KytyPS5/KytyPS5/commit/afbb322b323ce0415f163199c44d23230cf15bb2) |
| Audio | La cola modelada y el dispositivo aplican dos esperas. Un puerto de vibración también activa la espera alternativa de un lote con altavoz. | El audio síncrono con PCM real usa el reloj del dispositivo; los envíos asíncronos y sin dispositivo conservan límites. | [f380d69b](https://github.com/KytyPS5/KytyPS5/commit/f380d69b9d65290eaf85edc0f0e8faaf4591bc9c) |
| Compatibilidad del launcher | La descarga de `Nmzik/KytyPS5/releases/.../compatibility_db.json` devuelve HTTP 404. | Se usa la [fuente actual](https://kytyps5.github.io/data/compatibility.json), manteniendo reintentos, timeout y límite de 5 MiB. | [0bbe3011](https://github.com/KytyPS5/KytyPS5/commit/0bbe3011203f41ff39577cb483bb6bd9fc228a5c) |

Son backports puntuales, adaptados al nombre Katy. No se sustituye la base del
emulador. No se han medido FPS ni confirmado nuevos juegos jugables.
Los informes de la base de compatibilidad describen upstream; no prueban que
la misma versión de un juego funcione en este fork.

## Mejoras propias de validación

- `katy_tests` omitía `audio_out2_port_tests` aunque CTest lo registraba. Se añade
  su dependencia para que construir la suite también construya ese ejecutable.
- CI compilaba/probaba audio y memoria virtual, pero omitía las colas de eventos.
  Se añaden eventos y filesystem en Windows/Linux/macOS manteniendo los checks anteriores.
- Nuevos tests de filesystem verifican reutilización de descriptores, import con
  biblioteca/versión exactas, conservación de datos y posición tras truncar, y
  acceso **desde otro hilo** después de un seek inválido. El hilo distinto es
  necesario porque el mutex recursivo de Windows puede ocultar el bloqueo.
- Los tests afectados tienen timeout de 60 segundos para que un deadlock falle
  de forma acotada en CI.
- Se añade un test del planificador de audio de producción con SDL2/reloj deterministas
  para comprobar lotes con PCM ausente, mezclas altavoz/vibración y espera acotada
  de una cola detenida. Se amplían pruebas de eventos/audio, incluyendo lecturas de capacidad uno,
  eventos de nivel/borde, cola asíncrona llena y puertos exclusivos de vibración.

## Próximas mejoras, por orden propuesto

Estas son propuestas de implementación, no cambios ya incorporados.

Actualización 2026-10-13: la persistencia y validación de **SaveDataMemory2**
se incorporó en [v0.4.0](https://github.com/erickson558/KatyPS5/releases/tag/v0.4.0),
con separación por juego/usuario/slot, snapshots atómicos, respaldo e importación
de archivos de memoria en la misma ruta. Los guardados clásicos conservan sus
rutas. Ver [uso y recuperación](save-memory.md) y [SDD 003](../specs/003-save-memory-persistence/spec.md).
Las demás propuestas de este apartado siguen pendientes.

| Prioridad | Mejora | Evidencia upstream | Qué revisar antes de incorporarla |
| --- | --- | --- | --- |
| 1 | Guardados persistentes `SaveDataMemory2`. | [cfbaee07](https://github.com/KytyPS5/KytyPS5/commit/cfbaee07f0befa1a16b9c27528e9517742e135fb), con regresiones y una corrección de progreso de Astro Bot. | Compatibilidad con guardados previos, separación por usuario y reinicios. No copiar cambios de rutas sin probar migración/conservación. |
| 2 | Carga dinámica de módulos: refrescar imports y preservar relocaciones resueltas. | [ba579088](https://github.com/KytyPS5/KytyPS5/commit/ba579088), [1371219f](https://github.com/KytyPS5/KytyPS5/commit/1371219f), [49716600](https://github.com/KytyPS5/KytyPS5/commit/49716600). | Orden de carga/descarga, propiedad de módulos y stubs del fork; reproducir imports fallidos con un juego y log completos. |
| 3 | Eliminar el registro global de esperas que puede bloquear entrega de excepciones. | [18a1e0fa](https://github.com/KytyPS5/KytyPS5/commit/18a1e0fadc56b08f996a14a4ab2c71c4563154e8). | Adaptar conjuntamente `threads`, `pthread` y `libKernel`; verificar esperas, señales y carreras. El registro todavía existe en el fork. |
| 4 | Cinématicas: decodificación acotada en workers y reciclaje de buffers. | [091e6b31](https://github.com/KytyPS5/KytyPS5/commit/091e6b31), [e0b682e4](https://github.com/KytyPS5/KytyPS5/commit/e0b682e4), [a6f4e8e7](https://github.com/KytyPS5/KytyPS5/commit/a6f4e8e7f456b8697d3102823d3f16400c3e121e). | Estado de AvPlayer, timestamps, bucles, EOF, formato/padding de vídeo y versiones FFmpeg. |
| 5 | Audio: cadencia estable, NGS2 y soporte PCM/ATRAC9 más completo. | [70a475ec](https://github.com/KytyPS5/KytyPS5/commit/70a475ec8cd08575a41b31d234578e9c3e672dfa), [303a0203](https://github.com/KytyPS5/KytyPS5/commit/303a0203), [82b2711c](https://github.com/KytyPS5/KytyPS5/commit/82b2711c). | Adaptación SDL2/SDL3 y pruebas deterministas del backend real; los mocks de AudioOut2 no comprueban por sí solos la cadencia de SDL. |
| 6 | Reservas de memoria del driver antes de asignación directa del juego. | [9a5dd9fa](https://github.com/KytyPS5/KytyPS5/commit/9a5dd9faff1046f5f0d46b6355a5fa8894b00e36). | APIs de AGC y gestor de memoria actuales; casos de presión y fragmentación. |
| 7 | Texturas comprimidas, alias de profundidad y recompilación de shaders. | [dad900c6](https://github.com/KytyPS5/KytyPS5/commit/dad900c6), [f28c0fcc](https://github.com/KytyPS5/KytyPS5/commit/f28c0fcc), [984cfceb](https://github.com/KytyPS5/KytyPS5/commit/984cfceb). | Dependencias del IR y caché actuales; validación SPIR-V/Vulkan y capturas comparables. Son cambios de mayor alcance. |

Para medir mejoras de compatibilidad del fork conviene mantener una matriz
propia por juego: versión/commit, sistema/GPU/driver, estado (arranque/menú/juego),
guardado tras reinicio, audio, cinemáticas y log de fallos. Primero se fija un caso
reproducible; después se seleccionan backports que lo atiendan y se compara con
la versión anterior en el mismo equipo.

## Validación y límites

- Se revisó el diff contra las correcciones upstream y sus dependencias inmediatas.
- La URL nueva respondió HTTP 200; se verificaron 325 entradas, `status`/`comment`
  compatibles, estados reconocidos y respuesta menor al límite. La URL anterior
  respondió HTTP 404. Esto prueba el endpoint y su esquema, no la ejecución Qt.
- La validación local cubre formato C++, YAML, estructura/referencias CMake,
  selección de tests por CI, sincronización de versión y limpieza del diff.
- Compilación y regresiones completas en CI:
  [38324027422](https://github.com/erickson558/KatyPS5/actions/runs/38324027422),
  código `b8eb8d59`: Windows 5/5, Linux 7/7, macOS 5/5 suites y tres artefactos.
  Las pruebas de audio usan dispositivos y reloj simulados.
  Se necesita además una prueba con audio real y juegos antes de afirmar mejoras
  de sonido o compatibilidad observadas.
- Versión: **0.3.2**, [publicada](https://github.com/erickson558/KatyPS5/releases/tag/v0.3.2)
  el 2026-10-13. Publicación autorizada por el propietario el 2026-10-12.
- La revisión humana del diff es obligatoria antes de push según
  [`specs/CONSTITUTION.md`](../specs/CONSTITUTION.md), §8.

Se publicó la rama y se ejecutó **Build and Release KatyPS5**
con `workflow_dispatch` sobre ella. El workflow solo recibe push automático de
`main`/`master`; un push de esta rama sin PR/dispatch no dispara la compilación.
El dispatch no publica release porque la publicación requiere un evento push
en la rama canónica. Se integró por fast-forward después de builds/tests y CodeQL satisfactorios.

Comprobación reproducible con el toolchain:

```powershell
cmake --build _Build/windows --target katy_tests --parallel
ctest --test-dir _Build/windows --output-on-failure -R "^(audio_out2_port|audio_out_timing|event_queue_lifetime|kernel_file_system|virtual_memory_allocation)$"
```

En Linux/macOS, usar el directorio de compilación de la plataforma correspondiente.

## Entrega local para revisión

El propietario autorizó commit/push y recompilación el 2026-10-12, incluyendo
correcciones que surjan de la revisión por agentes y de CI.
El diff completo, incluidos archivos nuevos, se entrega en
`_Build/review/katyps5-0.3.2.patch` (artefacto local ignorado por Git).
Mensaje del commit de implementación:

```text
fix: backport upstream compatibility and stability fixes (v0.3.2)
```

Operaciones realizadas: `git status`/`git remote -v` para comprobar estado y
remotos; `git fetch upstream --prune` para actualizar referencias; `git merge-base`,
`git log` y `git diff` para comparar; `git switch -c fix/upstream-stability-0.3.2`
para aislar el trabajo; `git apply --check`/`git apply` para los backports que
encajaban y adaptaciones revisadas para los restantes; validación estática y
HTTP descritas arriba. El formateador se instaló únicamente bajo `_Build`.

## Revisión por agentes

- Código: el lote debe incluir PCM en un dispositivo real para usar su reloj.
  Se corrigió la condición y se añadió un test del planificador real; mocks de
  AudioOut2 por sí solos no cubren este caso.
- CI: target nuevo usa el TU de test que incluye audio.cpp, timer.cpp, common y
  fmt; no enlaza SDL2 porque sustituye su frontera de dispositivos. Se revisaron
  firmas, ABI y selección de suites en las tres plataformas.
- Publicación: install debe llevar LICENSE y LICENSES junto a los binarios.
  Se añadieron sus reglas sin modificar los avisos originales. El workflow
  existente crea tags fechados; la publicación semver v0.3.2 usará artefactos
  oficiales de la ejecución satisfactoria del commit integrado en main.

## Fallo detectado y corregido por CI

La primera ejecución compiló en las tres plataformas, pero las lecturas de
`KernelFileSystemTests` abortaron en `GetGpuResources`: `KernelRead` invalida
memoria aun sin renderer. Inicializar MemorySubsystem no instala ese gestor.
`b8eb8d59` omite la invalidación si no existen cachés GPU, conservando la ruta
anterior cuando están instaladas. La segunda ejecución aprobó las 17 suites.

Las actualizaciones de docs/specs/CHANGELOG/CLAUDE no disparan otra compilación
ni release por push. PR, dispatch y análisis programados siguen disponibles;
fuentes, tests, recursos, licencias, dependencias y workflows sí activan CI.

## Cierre de publicación — 2026-10-13

Implementación integrada en main `bc5211aa`,
[compilación oficial](https://github.com/erickson558/KatyPS5/actions/runs/38391264080)
y [CodeQL](https://github.com/erickson558/KatyPS5/actions/runs/38391264142)
satisfactorios. Las 17 suites también aprobaron en los builds oficiales.
[Release v0.3.2](https://github.com/erickson558/KatyPS5/releases/tag/v0.3.2)
publicada con tag anotado en ese SHA y los tres paquetes de esa ejecución exacta.
Los hashes SHA256 publicados coinciden con los descargados; los avisos de licencia
están incluidos y coinciden con los originales (normalizando fin de línea).

El ejecutable oficial Windows abrió con `--help`, salida 0, versión 0.3.2 y git
bc5211a en este equipo. Launcher local:
`_Build/releases/v0.3.2/windows/launcher.exe`. El paquete previo de rama se mantiene
en `_Build/preview-b8eb8d59/windows` para comparación. No se han probado juegos
ni medido rendimiento gráfico. Las propuestas de próximos backports siguen pendientes.

Comandos finales: `git merge --ff-only` integró la rama; `git push origin main`
activó compilación oficial; `git tag -a v0.3.2` y `git push origin v0.3.2` publicaron
la etiqueta; `gh release create ... --verify-tag --notes-file ...` publicó los
assets verificados. El código de esta release es inmutable; este cierre SDD se
registra en un commit documental posterior que no modifica fuentes ni binarios.
