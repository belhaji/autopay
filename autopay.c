#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>


#define USERS_FILE  "users.db"
#define CAISSE_FILE "caisse.db"
#define LOG_FILE    "autopay.log"
#define CONFIG_FILE "autopay.conf"


#define USERNAME_LEN 21
#define PASSWORD_LEN 100
#define NAME_LEN 41
#define CIN_LEN 11


typedef  enum { NORMAL , SUBSCRIBER} ClientType;

typedef enum { CAT1, CAT2, CAT3 } CategoryType;


enum
{
  COL_USERNAME = 0,
  COL_PASSWORD,
  COL_FNAME,
  COL_CIN,
  COL_ADMIN,
  COL_ACTIVE,
  NUM_COLS
};

typedef struct {
     CategoryType type;
     ClientType client;
    float price;
} Category;


typedef struct {
    int id;
    double price;
    Category category;
    ClientType type;
} Ticket;


typedef struct {
    int nextTicketId;
    Category cat1_normal;
    Category cat2_normal;
    Category cat3_normal;
    
    Category cat1_subscriber;
    Category cat2_subscriber;
    Category cat3_subscriber;
    
} Configuration;


typedef struct {
    char fullname[NAME_LEN];    // full name of the employee
    char cin[CIN_LEN];          // national code identifier
    gchar username[USERNAME_LEN];// user login name
    char password[PASSWORD_LEN];// user password
    short isAdmin;
    short isActive;             // user privilege 0 if is encaissier 1 if is administrator
} Employee ;





char* date_time_get(void);




//--------------- Log -------------------//

/*
 log entry example
 time date : user(id) : msg
 ex:
 13:34:56 56/11/2014 : ahmed (cin = AE345543) : a encaisser 567dh
 */

int log_action(FILE *file, Employee *emp,Ticket* tkt,char *msg);

int log_clear(FILE** file);

void log_init();

// ------------------------- Caisse ----------------------//

int caisse_add(FILE *file, double price);

int caisse_sub(FILE *file,double  price);

double caisse_get(FILE *file);


//---------------------- Category --------------------//




int category_set_price(FILE *file,  CategoryType type, ClientType ctype, float price);

double category_get_price(FILE *file,  CategoryType type,  ClientType ct);



// ---------------- APP Users --------------//


Employee* employee_new(char *fname, char *cin, char *username, char *password, short isAdmin, short isActive);

void employee_free(Employee *emp);


int employee_add(FILE *file, Employee *emp);


int employee_get_pos(FILE* file, char *cin);


int employee_exist(FILE *file, char *cin);


Employee* employee_get(FILE *file, char *cin);


int employee_edit(FILE *file, char *cin,Employee *emp);



//------------- Ticket --------------//


Ticket* ticket_new(FILE* file, double price, Category cat, ClientType client);

void ticket_free(Ticket * tkt);

int ticket_get_next_id(FILE* file);



//--------------- UI definition --------------//




typedef struct 
{
    GtkWidget *window;
    GtkWidget *btnExit;
    GtkWidget *btnSettings;
    GtkWidget *btnLog;
    GtkWidget *btnUsers;
}MainWindow;

typedef struct 
{
    GtkWidget *window;
    GtkWidget *txtUsername;
    GtkWidget *txtPassword;
    GtkWidget *btnCancel;
    GtkWidget *btnLogin;
} LoginWindow;

typedef struct 
{
    GtkWidget *window;
    GtkWidget *textView;
    GtkWidget *btnClose;
    GtkWidget *btnClear;
} LogDialog;

typedef struct 
{
    GtkWidget   *window,
                *treeView;
    GtkListStore    *store;
} UsersDialog;

typedef struct 
{
    MainWindow mainWindow;
    LoginWindow loginWindow;
    LogDialog logDialog;
    UsersDialog usersDialog;
    FILE* configFile;
    FILE* usersFile;
    FILE* caisseFile;
    FILE* logFile;
} App;


void app_init(App*);

void app_quit(App*);


void config_init(void);
void employee_init(void);
void casse_init(void);
void log_file_init(void);



void admin_window(App *app);

void login_window(App *app);

void login(GtkWidget* widget, gpointer data);



void log_dialog(App *app);
void show_log(GtkWidget* widget, gpointer data);
void close_log_window(GtkWidget* widget,gpointer data);
void clear_log_action(GtkWidget *widget,gpointer data);



void users_dialog(App *app);
void show_users_dialog(GtkWidget*,gpointer);
void users_dialog_close(GtkWidget *widget, gpointer data);


int main(int argc, char  *argv[])
{
    App app;

    gtk_init(&argc, &argv);

    app_init(&app);

    login_window(&app);

    gtk_main();
    
    app_quit(&app);


    return 0;
}




void config_init(void){

    FILE *file = fopen(CONFIG_FILE,"w");
    if (!file)
    {
        fprintf(stderr, "cannot open file : %s\n",CONFIG_FILE);
        return;
    }
    Configuration c;
    c.nextTicketId = 1;

    Category cat;
    cat.type = CAT1;
    cat.client = NORMAL;
    cat.price = 10.0f;
    c.cat1_normal = cat;

    cat.type = CAT2;
    cat.client = NORMAL;
    cat.price = 15.0f;
    c.cat2_normal = cat;

    cat.type = CAT3;
    cat.client = NORMAL;
    cat.price = 20.0f;
    c.cat3_normal = cat;

    cat.type = CAT1;
    cat.client = SUBSCRIBER;
    cat.price = 7.0f;
    c.cat1_subscriber = cat;

    cat.type = CAT2;
    cat.client = SUBSCRIBER;
    cat.price = 10.0f;
    c.cat2_subscriber = cat;

    cat.type = CAT3;
    cat.client = SUBSCRIBER;
    cat.price = 13.0f;
    c.cat3_subscriber = cat;

    fwrite(&c,sizeof(Configuration),1,file);
    fclose(file);
}

void casse_init(void){
    FILE *file = fopen(CAISSE_FILE,"w");
    if (!file)
    {
        fprintf(stderr, "cannot open file : %s\n",CAISSE_FILE);
        return;
    }
    double casse = 0;
    fwrite(&casse,sizeof(double),1,file);
    fclose(file);
}

void employee_init(void){
    FILE* file = fopen(USERS_FILE,"w");
    if(!file){
        fprintf(stderr, "cannot open file: %s\n", USERS_FILE);
        return;
    }
    Employee *emp = employee_new("utilisateur admin","AE12345678","admin","admin",1,1);
    employee_add(file,emp);
    employee_free(emp);
   fclose(file);
}

void log_file_init(void){
    FILE* file = fopen(LOG_FILE,"w");
    if(!file){
        fprintf(stderr, "cannot open file: %s\n", LOG_FILE);
        return;
    }
   fclose(file);
}


gchar* date_time_get(){
    
    GDateTime *datetime = g_date_time_new_now_local();
    gchar *str = g_date_time_format(datetime,(const gchar*) "%H:%M:%S %d/%m/%Y");
    g_date_time_unref(datetime);
    return str;
}



void log_init(){

}

int log_action(FILE *file, Employee *emp,Ticket* tkt,char *msg){
    
    gchar *time = date_time_get();
    
    g_fprintf(file, "%s : user(%s) : ticket(%d) : %s \n",
            time,
            emp->username,
            tkt->id,
            msg);
    
    if(time){
        g_free(time);
    }
    return 1;
}


int log_clear(FILE** file){
    if ((*file)) {
        fclose(*file);
        unlink(LOG_FILE);
        *file = fopen(LOG_FILE, "a");
        return 1;
    }
    return -1;
    
}


int log_get_text(FILE* file,char **text,int *len){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",LOG_FILE);
        return -1;
    }
    fseek(file,0,SEEK_END);
    *len = (int) ftell(file);
    fseek(file,0,SEEK_SET);
    *text = (char*) malloc( ( (*len) +1 ) * sizeof(char) );
    fread(*text, sizeof(char), *len, file);
    (*text)[ (*len) ] = '\0';
    return 1;
}



int caisse_add(FILE *file, double price){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    double caisse;
    fseek(file, 0, SEEK_SET);
    fread(&caisse, sizeof(double), 1, file);
    caisse += price;
    fseek(file, 0, SEEK_SET);
    fwrite(&caisse, sizeof(double), 1, file);
    return 0;
}


int caisse_sub(FILE *file,double  price){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    double caisse;
    fseek(file, 0, SEEK_SET);
    fread(&caisse, sizeof(double), 1, file);
    caisse -= price;
    fseek(file, 0, SEEK_SET);
    fwrite(&caisse, sizeof(double), 1, file);
    return 0;
}


double caisse_get(FILE *file){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    double caisse;
    fseek(file, 0, SEEK_SET);
    fread(&caisse, sizeof(double), 1, file);
    return caisse;
}


int category_set_price(FILE *file,  CategoryType catType, ClientType ctype, float price){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CONFIG_FILE);
        return -1;
    }
    Configuration c;
    fseek(file, 0, SEEK_SET);
    fread(&c, sizeof(Configuration), 1, file);
    switch (catType) {
        case CAT1:
            if (ctype == NORMAL) {
                c.cat1_normal.price = price;
            }else{
                c.cat1_subscriber.price = price;
            }
            break;
            
        case CAT2:
            if (ctype == NORMAL) {
                c.cat2_normal.price = price;
            }else{
                c.cat2_subscriber.price = price;
            }
            break;
        case CAT3:
            if (ctype == NORMAL) {
                c.cat3_normal.price = price;
            }else{
                c.cat3_subscriber.price = price;
            }
            break;

    }
    fseek(file, 0, SEEK_SET);
    fwrite(&c, sizeof(Configuration), 1, file);
    return 1;
}


double category_get_price(FILE *file,  CategoryType catType,  ClientType ct){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CONFIG_FILE);
        return -1;
    }
    Configuration c;
    fseek(file, 0, SEEK_SET);
    fread(&c, sizeof(Configuration), 1, file);
    switch (catType) {
        case CAT1:
            if (ct == NORMAL) {
                return c.cat1_normal.price;
            }else{
                return c.cat1_subscriber.price;
            }
            break;
            
        case CAT2:
            if (ct == NORMAL) {
               return c.cat2_normal.price;
            }else{
                return c.cat2_subscriber.price;
            }
            break;
        case CAT3:
            if (ct == NORMAL) {
               return c.cat3_normal.price;
            }else{
                return c.cat3_subscriber.price;
            }
            break;
        default:
            return  -1;
            break;
    }
}


Employee* employee_new(char *fname, char *cin, gchar *username, char *password, short isAdmin, short isActive){
    Employee *emp = (Employee*) malloc(sizeof(Employee));
    if (emp) {
        strncpy(emp->fullname, fname, NAME_LEN-1);
        strncpy(emp->cin, cin, CIN_LEN-1);
        g_strlcpy(emp->username, username, USERNAME_LEN-1);
        strncpy(emp->password, password, PASSWORD_LEN-1);
        emp->isAdmin = isAdmin;
        emp->isActive = isActive;
        return emp;
    }
    return NULL;
}

void employee_free(Employee *emp){
    if (emp) {
        free(emp);
    }
}


int employee_add(FILE *file, Employee *emp){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    fseek(file, 0, SEEK_END);
    fwrite(emp, sizeof(Employee), 1, file);
    return 1;
}


int employee_get_pos(FILE* file, gchar *username){
    int i;
    Employee e;
    fseek(file, 0, SEEK_SET);
    for (i=1 ; fread(&e, sizeof(Employee), 1, file) > 0 ; i++) {
        if (g_strcmp0(e.username, username) == 0) {
            return i;
        }
    }
    return -1;
}


int employee_exist(FILE *file, gchar *username){

    return employee_get_pos(file, username) != -1;
}


Employee* employee_get(FILE *file, gchar *username){
    int pos = employee_get_pos(file, username);
    if (pos == -1) {
        return NULL;
    }
    Employee *emp = (Employee*) malloc(sizeof(Employee));
    fseek(file, sizeof(Employee) * (pos-1), SEEK_SET);
    fread(emp  , sizeof(Employee), 1, file);
    return emp;
}


int employee_edit(FILE *file, gchar *username,Employee *emp){
    int pos;
    if (employee_exist(file, username)) {
        pos = employee_get_pos(file, username);
        fseek(file, sizeof(Employee) * (pos-1), SEEK_SET);
        fwrite(emp, sizeof(Employee), 1, file);
    }
    return -1;
}


Ticket* ticket_new(FILE* file, double price, Category cat, ClientType client){
    Ticket *tkt = (Ticket*) malloc(sizeof(Ticket));
    tkt->id = ticket_get_next_id(file);
    tkt->price = price;
    tkt->category = cat;
    tkt->type = client;
    return tkt;
}

int ticket_get_next_id(FILE *file){
    int id;
    if(!file){
        fprintf(stderr, "file not opened %s\n", CONFIG_FILE);
        return -1;
    }
    Configuration c;
    fseek(file, 0, SEEK_SET);
    fread(&c, sizeof(Configuration), 1, file);
    id = c.nextTicketId++;
    fseek(file,0,SEEK_SET);
    fwrite(&c,sizeof(Configuration),1,file);

    return id;
}


void ticket_free(Ticket * tkt){
    if (tkt) {
        free(tkt);
    }
}


void admin_window(App *app){
    GtkWidget   *window,
                *image,
                *vbox,
                *btnExit,
                *btnSettings,
                *btnLog,
                *btnUsers;


    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window),900,600);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

    btnLog = gtk_button_new();
    image = gtk_image_new_from_file("img/log.png");
    gtk_button_set_image(GTK_BUTTON(btnLog),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnLog,TRUE,FALSE,0);

    btnUsers = gtk_button_new();
    image = gtk_image_new_from_file("img/users.png");
    gtk_button_set_image(GTK_BUTTON(btnUsers),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnUsers,TRUE,FALSE,0);


    btnSettings = gtk_button_new();
    image = gtk_image_new_from_file("img/settings.png");
    gtk_button_set_image(GTK_BUTTON(btnSettings),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnSettings,TRUE,FALSE,20);

    btnExit = gtk_button_new();
    image = gtk_image_new_from_file("img/exit.png");
    gtk_button_set_image(GTK_BUTTON(btnExit),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnExit,TRUE,FALSE,20);

    gtk_container_add(GTK_CONTAINER (window), vbox);

    g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL); 
    g_signal_connect(btnExit, "clicked",G_CALLBACK(gtk_main_quit), NULL); 
    g_signal_connect(btnLog, "clicked",G_CALLBACK(show_log), (gpointer) app); 
    g_signal_connect(btnUsers, "clicked",G_CALLBACK(show_users_dialog), (gpointer) app); 



    app->mainWindow.window = window;
    app->mainWindow.btnExit = btnExit;
    app->mainWindow.btnSettings = btnSettings;
    app->mainWindow.btnLog = btnLog;
    app->mainWindow.btnUsers = btnUsers;


    gtk_widget_show_all(window);    
}

void login_window(App *app){
    GtkWidget   *loginWindow,
                *lblUsername,
                *lblPassword,
                *txtUsername,
                *txtPassword,
                *btnCancel,
                *btnLogin,
                *grid,
                *hbox,
                *vbox;


    loginWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(loginWindow),GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_resizable(GTK_WINDOW(loginWindow),FALSE);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    grid = gtk_grid_new();
    
    lblUsername = gtk_label_new("Username");
    lblPassword = gtk_label_new("Password");
    txtUsername = gtk_entry_new();
    txtPassword = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(txtPassword),FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(txtPassword),'*');
    btnCancel   = gtk_button_new_with_label("Cancel");
    btnLogin    = gtk_button_new_with_label("Login");

    gtk_box_pack_start(GTK_BOX(hbox), btnCancel,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(hbox), btnLogin,TRUE,TRUE,0);
   
    gtk_grid_attach(GTK_GRID(grid),lblUsername,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),lblPassword,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),txtUsername,1,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),txtPassword,1,1,1,1);

    gtk_grid_set_row_spacing(GTK_GRID(grid),10);
    gtk_grid_set_column_spacing(GTK_GRID(grid),10);

    gtk_box_pack_start(GTK_BOX(vbox), grid,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox,FALSE,FALSE,0);



    gtk_container_add(GTK_CONTAINER(loginWindow),vbox);

    g_signal_connect(G_OBJECT(btnLogin),"clicked",G_CALLBACK(login),app);
    g_signal_connect(loginWindow, "destroy",G_CALLBACK(gtk_main_quit), NULL); 
    g_signal_connect(btnCancel, "clicked",G_CALLBACK(gtk_main_quit), NULL); 





    app->loginWindow.window = loginWindow;
    app->loginWindow.txtUsername = txtUsername;
    app->loginWindow.txtPassword = txtPassword;
    app->loginWindow.btnLogin = btnLogin;
    app->loginWindow.btnCancel = btnCancel;

    gtk_widget_show_all(loginWindow);
}


void login(GtkWidget* widget, gpointer data){
    
    App *app = (App*) data;
    Employee *emp;
    gchar *username = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->loginWindow.txtUsername));
    gchar *password = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->loginWindow.txtPassword));

    if(employee_exist(app->usersFile,username)){
        emp = employee_get(app->usersFile,username);
        if (strcmp(emp->password,password) == 0 && emp->isActive)
        {
            g_print("login success\n");
            admin_window(app);
            gtk_widget_hide(app->loginWindow.window);

        }else{
            GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(app->loginWindow.window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "nom de l'utilisateur ou le modepass incorrect !");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
        employee_free(emp);

    }else{
            GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(app->loginWindow.window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "nom de l'utilisateur ou le modepass incorrect !");

            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
    }  
}

void app_init(App *app){

    app->usersFile = fopen(USERS_FILE,"r+");
    if(!app->usersFile){
        employee_init();
        app->usersFile = fopen(USERS_FILE,"r+");
    }
    app->logFile = fopen(LOG_FILE,"a+");
    if(!app->logFile){
        log_file_init();
        app->logFile = fopen(LOG_FILE,"a+");
    }
    app->configFile = fopen(CONFIG_FILE,"r+");
    if(!app->configFile){
        config_init();
        app->configFile = fopen(CONFIG_FILE,"r+");
    }
    app->caisseFile = fopen(CAISSE_FILE,"r+");
    if(!app->caisseFile){
        casse_init();
        app->caisseFile = fopen(CAISSE_FILE,"r+");
    }
}

void app_quit(App *app){
    fclose(app->configFile);
    fclose(app->usersFile);
    fclose(app->logFile);
    fclose(app->caisseFile);
}

void log_dialog(App *app){
    GtkWidget   *window,
                *textView,
                *vbox,
                *hbox,
                *btnClear,
                *btnClose,
                *scrollWindow;
    GtkTextBuffer *buffer;
    PangoFontDescription *font_desc;


    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    btnClose = gtk_button_new_with_label("Fermer");
    btnClear = gtk_button_new_with_label("Supprimer tout");
    textView = gtk_text_view_new();
    scrollWindow = gtk_scrolled_window_new(NULL,NULL);


    gtk_text_view_set_editable(GTK_TEXT_VIEW(textView),FALSE);
    gtk_text_view_set_cursor_visible    (GTK_TEXT_VIEW(textView),FALSE);
    font_desc = pango_font_description_from_string ("Monospace 15");
    gtk_widget_override_font (textView, font_desc);
    pango_font_description_free (font_desc);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textView));

    gsize len;
    gchar *text;
    g_file_get_contents(LOG_FILE,&text,&len,NULL);
    gtk_text_buffer_set_text(buffer,text,-1);

    


    gtk_window_set_title(GTK_WINDOW(window),"LOGS DIALOG");
    gtk_window_set_default_size(GTK_WINDOW(window),900,600);
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->mainWindow.window));


    gtk_container_add(GTK_CONTAINER(scrollWindow), textView);

    gtk_box_pack_start(GTK_BOX(hbox),btnClear,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox), btnClose,FALSE,FALSE,5);
    
    gtk_box_pack_start(GTK_BOX(vbox), hbox,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox), scrollWindow,TRUE,TRUE,0);

    gtk_container_add(GTK_CONTAINER(window),vbox);



    g_signal_connect(G_OBJECT(btnClose),"clicked",G_CALLBACK(close_log_window),(gpointer) app);
    g_signal_connect(G_OBJECT(btnClear),"clicked",G_CALLBACK(clear_log_action),(gpointer) app);

    app->logDialog.window = window;
    app->logDialog.textView = textView;
    app->logDialog.btnClose = btnClose;
    app->logDialog.btnClear = btnClear;

    gtk_widget_show_all(window);
}


void show_log(GtkWidget* widget, gpointer data){
    log_dialog((App*) data);
}

void close_log_window(GtkWidget* widget,gpointer data){
    App * app = (App*) data;
    gtk_window_close(GTK_WINDOW(app->logDialog.window));
}

void clear_log_action(GtkWidget *widget,gpointer data){
    App *app = (App*) data;
    log_clear( &(app->logFile));
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->logDialog.textView)),"",-1);
}


void users_dialog(App* app){
    GtkWidget   *window,
                *treeView,
                *btnEdit,
                *scrollWindow,
                *btnAdd,
                *btnClose,
                *hbox,
                *vbox;

    GtkCellRenderer *cellRendrer;
    GtkListStore  *store;
    GtkTreeIter    iter;
    Employee    e;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    btnClose = gtk_button_new_with_label("Fermer");
    btnEdit = gtk_button_new_with_label("Editer");
    btnAdd = gtk_button_new_with_label("Ajouter");
    scrollWindow = gtk_scrolled_window_new(NULL,NULL);



    store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING,
                                          G_TYPE_STRING,
                                          G_TYPE_STRING,
                                          G_TYPE_STRING,
                                          G_TYPE_INT,
                                          G_TYPE_INT);

    if (!(app->usersFile))
    {
        fprintf(stderr, "file not opened : %s\n",USERS_FILE );
        exit(123);
    }
    fseek(app->usersFile,0,SEEK_SET);
    while(fread(&e,sizeof(Employee),1,app->usersFile) > 0){
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                      COL_FNAME, e.fullname,
                      COL_USERNAME, e.username,
                      COL_PASSWORD, e.password,
                      COL_CIN, e.cin,
                      COL_ACTIVE, e.isActive,
                      COL_ADMIN, e.isAdmin,
                      -1);
    }

    cellRendrer = gtk_cell_renderer_text_new();
    treeView = gtk_tree_view_new();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,      
                                               "Nom",  
                                               cellRendrer,
                                               "text", COL_FNAME,
                                               NULL);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,      
                                               "Username",  
                                               cellRendrer,
                                               "text", COL_USERNAME,
                                               NULL);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,      
                                               "Password",  
                                               cellRendrer,
                                               "text", COL_PASSWORD,
                                               NULL);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,      
                                               "CIN",  
                                               cellRendrer,
                                               "text", COL_CIN,
                                               NULL);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,      
                                               "Admin",  
                                               cellRendrer,
                                               "text", COL_ADMIN,
                                               NULL);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,      
                                               "Active",  
                                               cellRendrer,
                                               "text", COL_ACTIVE,
                                               NULL);
    gtk_tree_view_set_model (GTK_TREE_VIEW (treeView), GTK_TREE_MODEL(store));
    g_object_unref (store);

    gtk_window_set_title(GTK_WINDOW(window), "Utilisateurs");
    gtk_window_set_default_size(GTK_WINDOW(window),900,600);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->mainWindow.window));


    gtk_container_add(GTK_CONTAINER(scrollWindow),treeView);

    gtk_container_add(GTK_CONTAINER(window),vbox);

    gtk_box_pack_start(GTK_BOX(hbox),btnAdd,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(hbox),btnEdit,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(hbox),btnClose,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox),scrollWindow,TRUE,TRUE,0);

    g_signal_connect(btnClose, "clicked",G_CALLBACK(users_dialog_close), app); 



    app->usersDialog.window = window;
    app->usersDialog.treeView = treeView;
    app->usersDialog.store = store;



    gtk_widget_show_all(window);




}



void show_users_dialog(GtkWidget *widget, gpointer data){
    App *app = (App*) data;
    users_dialog(app);
}

void users_dialog_close(GtkWidget *widget, gpointer data){
    App *app = (App*) data;
    gtk_window_close(GTK_WINDOW(app->usersDialog.window));
}






























