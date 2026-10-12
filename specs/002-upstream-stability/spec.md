# Spec 002: Backport de compatibilidad y estabilidad

## Status
In Progress

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
La etiqueta/release se pospone hasta revisión humana y CI satisfactoria.
