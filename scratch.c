#define ui_value_type_pair(name) ui_value* name
#define computed_component(name, args, ...) \
    ui_widget* name(ui_value_type_pair(__VA_ARGS__))

typedef struct {
    int counter;
} ui_value;

ui_value cur_count;

computed_component(counter_label, (&cur_count)) {
    return ui_label(to_string(cur_count));
}