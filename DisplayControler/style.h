static lv_style_t style0;
static lv_style_t style1;
static lv_style_t style2;
static lv_style_t styleSeparatorHorizontal;
static lv_style_t style3;
static lv_style_t style4;
static lv_style_t style5;
static lv_style_t style6;
static lv_style_t style7;
static lv_style_t style8;
static lv_style_t style9;
static lv_style_t styleSeparatorVertical;
static lv_style_t style10;
static lv_style_t style11;
static lv_style_t style12;
static lv_style_t style13;
static lv_style_t style14;

static lv_style_t black_bg_style;

void initialStyle() {
  lv_style_init(&style0);
  lv_style_set_bg_color(&style0, lv_color_make(136, 153, 255));
  lv_style_set_border_color(&style0, lv_color_make(0, 0, 0));
  lv_style_set_width(&style0, 160);
  lv_style_set_height(&style0, 180);
  lv_style_set_radius(&style0, 71);

  lv_style_init(&style1);
  lv_style_set_bg_color(&style1, lv_color_make(136, 153, 255));
  lv_style_set_border_color(&style1, lv_color_make(136, 153, 255));
  lv_style_set_width(&style1, (1024 - 80));
  lv_style_set_height(&style1, 10);
  lv_style_set_radius(&style1, 0);

  lv_style_init(&style2);
  lv_style_set_bg_color(&style2, lv_color_make(204, 153, 255));
  lv_style_set_border_color(&style2, lv_color_make(204, 153, 255));
  lv_style_set_width(&style2, 90);
  lv_style_set_height(&style2, 45);
  lv_style_set_radius(&style2, 0);

  lv_style_init(&styleSeparatorHorizontal);
  lv_style_set_bg_color(&styleSeparatorHorizontal, lv_color_make(0, 0, 0));
  lv_style_set_border_color(&styleSeparatorHorizontal, lv_color_make(0, 0, 0));
  lv_style_set_width(&styleSeparatorHorizontal, 90);
  lv_style_set_height(&styleSeparatorHorizontal, 5);
  lv_style_set_radius(&styleSeparatorHorizontal, 0);

  lv_style_init(&style3);
  lv_style_set_bg_color(&style3, lv_color_make(0, 0, 0));
  lv_style_set_border_color(&style3, lv_color_make(0, 0, 0));
  lv_style_set_width(&style3, 58);
  lv_style_set_height(&style3, 145);
  lv_style_set_radius(&style3, 21);

  lv_style_init(&style4);
  lv_style_set_bg_color(&style4, lv_color_make(204, 68, 68));
  lv_style_set_border_color(&style4, lv_color_make(0, 0, 0));
  lv_style_set_width(&style4, 160);
  lv_style_set_height(&style4, 140);
  lv_style_set_radius(&style4, 71);

  lv_style_init(&style5);
  lv_style_set_bg_color(&style5, lv_color_make(204, 68, 68));
  lv_style_set_border_color(&style5, lv_color_make(204, 68, 68));
  lv_style_set_width(&style5, (1024 - 80));
  lv_style_set_height(&style5, 10);
  lv_style_set_radius(&style5, 0);

  lv_style_init(&style6);
  lv_style_set_bg_color(&style6, lv_color_make(204, 68, 68));
  lv_style_set_border_color(&style6, lv_color_make(204, 68, 68));
  lv_style_set_width(&style6, 90);
  lv_style_set_height(&style6, 140);
  lv_style_set_radius(&style6, 0);

  lv_style_init(&style7);
  lv_style_set_bg_color(&style7, lv_color_make(255, 136, 0));
  lv_style_set_border_color(&style7, lv_color_make(255, 136, 0));
  lv_style_set_width(&style7, 90);
  lv_style_set_height(&style7, 35);
  lv_style_set_radius(&style7, 0);

  lv_style_init(&style8);
  lv_style_set_bg_color(&style8, lv_color_make(0, 0, 0));
  lv_style_set_border_color(&style8, lv_color_make(0, 0, 0));
  lv_style_set_width(&style8, 58);
  lv_style_set_height(&style8, 145);
  lv_style_set_radius(&style8, 21);

  lv_style_init(&black_bg_style);
  lv_style_set_bg_color(&black_bg_style, lv_color_black());
  lv_obj_add_style(lv_scr_act(), &black_bg_style, 0);

  lv_style_init(&styleSeparatorVertical);
  lv_style_set_bg_color(&styleSeparatorVertical, lv_color_make(0, 0, 0));
  lv_style_set_border_color(&styleSeparatorVertical, lv_color_make(0, 0, 0));
  lv_style_set_width(&styleSeparatorVertical, 5);
  lv_style_set_height(&styleSeparatorVertical, 25);
  lv_style_set_radius(&styleSeparatorVertical, 0);

  lv_style_init(&style9);
  lv_style_set_bg_color(&style9, lv_color_make(255, 153, 102));
  lv_style_set_border_color(&style9, lv_color_make(255, 153, 102));
  lv_style_set_width(&style9, 90);
  lv_style_set_height(&style9, (600 - 390 - 40));
  lv_style_set_radius(&style9, 0);

  lv_style_init(&style10);
  lv_style_set_bg_color(&style10, lv_color_make(255, 153, 0));
  lv_style_set_border_color(&style10, lv_color_make(255, 153, 0));
  lv_style_set_width(&style10, 5);
  lv_style_set_height(&style10, 15);
  lv_style_set_radius(&style10, 0);

        

}
