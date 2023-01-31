idf_component_register(SRCS "bench.c" "stack.c" "../../framework.c"
                       PRIV_INCLUDE_DIRS "../.."
                       PRIV_REQUIRES cementite esp_timer)
