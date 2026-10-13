# Spec 002: Backport de compatibilidad y estabilidad

## Status
Done

## Context
El usuario pidió comparar KytyPS5 con KatyPS5 y aplicar mejoras, priorizando
compatibilidad y estabilidad. La base común es `0bbd0bdd`; upstream se revisó en
`df5b9b92` el 2026-10-12. Hay 1,167 commits pendientes y una migración a SDL3,
por lo que las correcciones independientes se adaptarán a la base SDL2 actual.

## Goal
Corregir seis problemas comprobables del kernel, audio y launcher, conservar el rebrand,
las traducciones y las opciones actuales, y preparar pruebas de regresión en CI.

## Approach
- `b9aa0dc3`: usar RAII en `KernelLseek`; un offset negativo actualmente retorna
  con el mutex tomado, bloqueando operaciones posteriores del mismo archivo.
- `0f3674f5`: liberar el descriptor al rechazar `O_RDONLY | O_TRUNC`; actualmente
  cada intento retiene un objeto y una entrada en la tabla de descriptores.
- `048fe9f4`: exportar el handler existente de `sceKernelFtruncate`; actualmente
  su NID no se resuelve aunque la implementación nativa ya existe.
- `afbb322b`: entregar eventos de nivel una vez por espera, manteniendo el orden
  de eventos pendientes; actualmente un evento persistente ocupa todo el lote.
- `f380d69b`: confiar en la cola del dispositivo para audio síncrono con PCM;
  conservar los límites de envíos asíncronos y la espera sin dispositivo real.
- `0bbe3011`: actualizar la fuente de compatibilidad del launcher. La URL antigua
  devuelve HTTP 404; la actual devuelve HTTP 200 y JSON compatible de 84,730 bytes
  al revisar el 2026-10-12. Se conservan timeout, reintentos y límite de 5 MiB.
- Registrar las pruebas nuevas y cubrir audio/eventos/filesystem en CI.
- Revisión por agentes: el reloj de un lote necesita dispositivo **y PCM**.
  El API público admite `ptr == nullptr`; SDL no espera ni encola en ese caso.
  Sin comprobar PCM, un dispositivo vacío junto a vibración elimina la única
  espera del lote. Se exige PCM al seleccionar reloj y se añade regresión del
  planificador de producción con backend determinista. Riesgo: ritmo de lotes
  mixtos; no modifica la ruta de lotes que contienen PCM real.
- Revisión de publicación: el paso install omitía LICENSE y LICENSES del paquete.
  Se incluyen los archivos existentes junto a los binarios para conservar sus
  avisos y atribución en los artefactos. Riesgo limitado al contenido del paquete;
  no modifica ni cambia las licencias ni la ejecución.
- Fallo comprobado en CI macOS `38322431144`: las lecturas de archivos en la
  regresión abortan en `memory.cpp:75`, porque `InvalidateMemory` exige un gestor
  GPU aunque el buffer sea de CPU y todavía no exista renderer. MemorySubsystem
  no instala ese gestor; lo instala el renderer. Se omite la invalidación cuando
  no hay recursos GPU, igual que MapGpuRange/UnmapGpuRange/HandleGpuFault.
  Impacto: permite I/O de CPU antes de instalar el renderer. Riesgo acotado: con
  recursos instalados sigue invalidando las cachés por la misma ruta; sin recursos
  no hay cachés GPU que invalidar. Las lecturas reales de la prueba FS cubren el
  caso sin renderer y se conserva la suite de memoria virtual.
- Cierre SDD: los pushes que solo actualizan docs, specs, CHANGELOG o CLAUDE
  actualmente reconstruyen tres plataformas y crean otra release fechada sin
  cambiar el programa. Se excluyen esos cuatro grupos de los triggers push de
  build/CodeQL. PR, dispatch y schedule conservan sus triggers. Código, tests,
  CMake/workflows, assets, dependencias y licencias siguen activando CI. Riesgo:
  omitir compilación de documentación; esos archivos no se usan para construir
  el emulador. Se valida YAML/lista exacta de exclusiones y la publicación de
  código en main verifica el trigger positivo.

## Risk / impact
Código compartido Windows/Linux/macOS. Riesgo principal: cambiar el ritmo del
audio o la entrega de eventos. Se adaptan pruebas upstream y se añaden pruebas
de llamadas posteriores a un seek inválido, reutilización de descriptores y
resolución del import. Se usarán timeouts de CTest para detectar bloqueos.
No cambia el formato de configuración, guardados ni CLI.

La revisión estática local es posible. No hay compilador, CMake, Qt ni SDK Vulkan
locales utilizables. La compilación y ejecución real requieren GitHub Actions.
El propietario autorizó publicar y recompilar el 2026-10-12, incluyendo las
correcciones adicionales que surjan de revisión/CI. Se usarán agentes de revisión
y compilación, conservando este registro SDD.

## Version impact
Patch: 0.3.1 -> 0.3.2. Se corrigen implementaciones e imports existentes.
Etiqueta anotada y release [v0.3.2](https://github.com/erickson558/KatyPS5/releases/tag/v0.3.2)
publicadas en `bc5211aac2fe2d81a4abe777488b464f080f1576` después de CI satisfactoria.
Cierre: 2026-10-13. Los commits posteriores de auditoría solo actualizan documentación.
