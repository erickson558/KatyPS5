# Plan 003

1. Analizar estado volátil, rangos y reglas upstream; escribir especificación.
2. Implementar almacenamiento acotado con reemplazo atómico y respaldo.
3. Integrar Setup/Get/Set/Sync y ciclo de vida con mutex/errores guest.
4. Añadir regresiones reales, incluyendo procesos separados y archivos heredados.
5. Registrar suite en CMake/CI y sincronizar versión 0.4.0/changelog.
6. Revisar almacenamiento, límites y diff con agentes; validar formato y YAML.
7. Publicar rama, compilar/probar tres plataformas y ejecutar CodeQL.
8. Integrar tras CI satisfactoria, publicar paquetes oficiales y cerrar SDD.
