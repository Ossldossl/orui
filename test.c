typedef struct {
    str full_name; // owns the data
    str_slice name, surname;
} person;

typedef struct {
  str filter;
  person input;
  // array of person(s)
  array person_list;
  array filtered_list;
  u32 selected;
} crud_model;

crud_model model;

void update(u32 msg, int di, void* dp) 
{
  switch (msg) {
    case MSG_SUBMIT_NAME: {
      model.input.name=(str*)dp;
      model.input.full_name=str_concat(model.input.name, ", ", model.input.surname);
    } break;
    case MSG_SUBMIT_SURNAME: {
      model.input.surname=(str*)dp;
      model.input.full_name=str_concat(model.input.name, ", ", model.input.surname);
    } break;
    case MSG_SELECT: {
      model.selected=di;
    } break;
    case MSG_CREATE: {
      person* new_p = array_push(model.person_list);
      new_p->name = model.input.name; new_p->surname = model.input.surname;
      new_p->full_name = str_concat(model.input.name, ", ", model.input.surname);
    } break;
    case MSG_UPDATE: {
      if (model.selected == -1) return;
      person* p = array_get(model.person_list, model.selected);
      p->name = model.input.name; p->surname = model.input.surname;
      p->full_name = str_concat(model.input.name, ", ", model.input.surname);
    } break;
    case MSG_DELETE: {
      if (model.selected == -) return;
      array_remove(model.person_list, model.selected);
    } break;
    case MSG_FILTER: {
      str filter = *(str*)dp;
      array_clear(model.filter_list);
      for_each(p, model.person_list) {
        if (str_contains(p->full_name, filter)) {
          person** copied_p = array_push(&model.filter_list);
          *copied_p = p;
        }
      }}
    } break;
  }
}

component(filter_header) {
  return textbox(
    .width=pct(100),
    .hint="Filter",
    .on_submit_msg=MSG_FILTER,
  );
}}

// ^= list builder
ui_widget* populate_list(composer* c, person** p) {
  return label(
    .width=pct(100),
    .text=(*p)->full_name
  );
}

component(left_side, list) {
  return column(
    .width=pct(75),
    .height=pct(100),
    children(
      filter_header(),
      lazy_list(
        .builder=populate_list,
        .data=list,
        .on_select_msg=MSG_SELECT
      ),
      row(
        .width=pct(100),
        children(
          button(.text="Create", .msg=MSG_CREATE),
          button(.text="Update", .msg=MSG_UPDATE),
          button(.text="Delete", .msg=MSG_DELETE)
        )
      )
    )
  );
}}

component(right_side) {
  return column(
    children(
      textbox(
        .width=pct(100),
        .hint="Name",
        .on_submit_msg=MSG_SUBMIT_NAME,
      ),
      textbox(
        .width=pct(100),
        .hint="Surname",
        .on_submit_msg=MSG_SUBMIT_SURNAME
      )
    )
  );
}}

typedef struct {
  bool is_pct;
  union {
    float pct;
    int val;
  };
} dp_t;

#define pct(value) (dp_t){.is_pct=true,.pct=value}
#define dp(value) (dp_t){.is_pct=false,.val=value}

typedef struct ui_modifier* {
  struct ui_modifier* child;
  struct ui_modifier* parent;
} ui_modifier;

#define __mod(a, b, ...) a, b,
#define modifiers(...) __mod(__VA_ARGS__)

widget* app(composer* c) {
  composer_set_update_handler(c, update);
  return row(
    modifiers(

    ),
    .height=pct(100), .width=pct(100),
    children(
      left_side(c, model.filtered_list),
      right_side(c)
    )
  );
}

void init() {
  model.filter = str_empty();
  model.person.name = str_empty(); model.person.surname = str_empty();
  model.filter_list = array_init(sizeof(person*), 0);
  model.person_list = array_init(sizeof(person), 5);
}
