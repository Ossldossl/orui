typedef struct ui_widget {} ui_widget;
typedef struct ui_modifier {struct ui_modifier* parent, *child; char* id; } ui_modifier;
typedef struct ui_button {ui_modifier* mod; char* text; } ui_button; 

#define button(...) &(ui_button){__VA_ARGS__}

ui_widget* test() 
{
    button(
        .text="hello, world!",
        modifiers(
            
        )
    )
}