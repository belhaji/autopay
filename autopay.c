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
    gdouble price;
} Category;


typedef struct {
    int id;
    Category category;
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
    gchar fullname[NAME_LEN];    // full name of the employee
    gchar cin[CIN_LEN];          // national code identifier
    gchar username[USERNAME_LEN];// user login name
    gchar password[PASSWORD_LEN];// user password
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

int log_action(FILE *file, gchar*,int ,gchar*);

int log_clear(FILE** file);

void log_init();

// ------------------------- Caisse ----------------------//

int caisse_add(FILE *file, gdouble price);

int caisse_sub(FILE *file,gdouble  price);

gdouble caisse_get(FILE *file);


//---------------------- Category --------------------//




int category_set_price(FILE *file,  CategoryType type, ClientType ctype, gdouble price);

gdouble category_get_price(FILE *file,  CategoryType type,  ClientType ct);



// ---------------- APP Users --------------//


Employee* employee_new(char *fname, char *cin, char *username, char *password, short isAdmin, short isActive);

void employee_free(Employee *emp);


int employee_add(FILE *file, Employee *emp);


int employee_get_pos(FILE* file, char *cin);


int employee_exist(FILE *file, char *cin);


Employee* employee_get(FILE *file, char *cin);


int employee_edit(FILE *file, char *cin,Employee *emp);



//------------- Ticket --------------//


Ticket* ticket_new(FILE* file, gdouble price, CategoryType cat, ClientType client);

void ticket_free(Ticket * tkt);

int ticket_get_next_id(FILE* file);



//--------------- UI definition --------------//




typedef struct 
{
    GtkWidget *window;
}MainWindow;

typedef struct 
{
    GtkWidget *window;
    GtkWidget *txtUsername;
    GtkWidget *txtPassword;
} LoginWindow;

typedef struct 
{
    GtkWidget *window;
    GtkWidget *textView;
} LogDialog;

typedef struct 
{
    GtkWidget   *window,
                *treeView;
    GtkListStore    *store;
} UsersDialog;

typedef struct 
{
     GtkWidget  *window,
                *switchIsAdmin,
                *switchIsActive,
                *entryUsername,
                *entryPassword,
                *entryCIN,
                *entryFullName;
} UsersDialogAdd;

typedef struct 
{
     GtkWidget  *window,
                *switchIsAdmin,
                *switchIsActive,
                *entryUsername,
                *entryPassword,
                *entryCIN,
                *entryFullName;
    GtkTreeSelection *selection;
} UsersDialogEdit;



typedef struct 
{
    GtkWidget   *window,
                *spinCat1Normal,
                *spinCat2Normal,
                *spinCat3Normal,
                *spinCat1Subscriber,
                *spinCat2Subscriber,
                *spinCat3Subscriber;
} SettingsDialog;

typedef struct 
{
    GtkWidget *window;

}EmployeeWindow;

typedef struct 
{
   GtkWidget    *window,
                *radioNormal,
                *radioSubscriber,
                *comboCategory; 
} TicketDialog;

typedef struct 
{
    GtkWidget   *window,
                *spinCurrentPrice;
} CheckOutDialog;

typedef struct 
{
    MainWindow mainWindow;
    LoginWindow loginWindow;
    LogDialog logDialog;
    UsersDialog usersDialog;
    UsersDialogAdd usersDialogAdd;
    UsersDialogEdit usersDialogEdit;
    SettingsDialog settingsDialog;
    EmployeeWindow employeeWindow;
    TicketDialog ticketDialog;
    CheckOutDialog checkOutDialog;
    FILE* configFile;
    FILE* usersFile;
    FILE* caisseFile;
    FILE* logFile;
    Employee logedUser;
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
void users_subdialog_add(App *app);
void users_subdialog_show(GtkWidget* widget,gpointer data);
void users_subdialog_add_save(GtkWidget* widget,gpointer data);
void users_subdialog_add_cancel(GtkWidget* widget,gpointer data);
void users_subdialog_edit(App *app);
void users_subdialog_edit_show(GtkWidget* widget,gpointer data);
void users_subdialog_edit_save(GtkWidget* widget,gpointer data);
void users_subdialog_edit_cancel(GtkWidget *widget,gpointer data);


void settings_dialog(App* app);
void settings_dialog_show(GtkWidget *widget,gpointer data);
void settings_dialog_save(GtkWidget *widget,gpointer data);
void settings_dialog_cancel(GtkWidget *widget,gpointer data);



void employee_window(App *app);
void ticket_dialog(App *app);
void ticket_dialog_show(GtkWidget *widget, gpointer data);
void ticket_dialog_ok(GtkWidget* widget, gpointer data);
void ticket_dialog_cancel(GtkWidget *widget, gpointer data);

void checkout_dialog(App*);
void checkout_dialog_show(GtkWidget*, gpointer*);
void checkout_dialog_ok(GtkWidget*, gpointer*);
void checkout_dialog_cancel(GtkWidget*, gpointer*);



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
    cat.price = 10.0;
    c.cat1_normal = cat;

    cat.type = CAT2;
    cat.client = NORMAL;
    cat.price = 15.0;
    c.cat2_normal = cat;

    cat.type = CAT3;
    cat.client = NORMAL;
    cat.price = 20.0;
    c.cat3_normal = cat;

    cat.type = CAT1;
    cat.client = SUBSCRIBER;
    cat.price = 7.0;
    c.cat1_subscriber = cat;

    cat.type = CAT2;
    cat.client = SUBSCRIBER;
    cat.price = 10.0;
    c.cat2_subscriber = cat;

    cat.type = CAT3;
    cat.client = SUBSCRIBER;
    cat.price = 13.0;
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
    gdouble casse = 0;
    fwrite(&casse,sizeof(gdouble),1,file);
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


int log_action(FILE *file, gchar * username,int tktId,gchar *msg){
    
    gchar *time = date_time_get();
    
    g_fprintf(file, "%s : user( %s ) : ticket( %d ) : %s \n",
            time,
            username,
            tktId,
            msg);
    fflush(file);
    
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

int caisse_add(FILE *file, gdouble price){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    gdouble caisse;
    fseek(file, 0, SEEK_SET);
    fread(&caisse, sizeof(gdouble), 1, file);
    caisse += price;
    fseek(file, 0, SEEK_SET);
    fwrite(&caisse, sizeof(gdouble), 1, file);
    fflush(file);
    return 0;
}

int caisse_sub(FILE *file,gdouble  price){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    gdouble caisse;
    fseek(file, 0, SEEK_SET);
    fread(&caisse, sizeof(gdouble), 1, file);
    caisse -= price;
    fseek(file, 0, SEEK_SET);
    fwrite(&caisse, sizeof(gdouble), 1, file);
    fflush(file);
    return 0;
}


gdouble caisse_get(FILE *file){
    if(!file){
        fprintf(stderr, "Error file not opened %s\n",CAISSE_FILE);
        return -1;
    }
    gdouble caisse;
    fseek(file, 0, SEEK_SET);
    fread(&caisse, sizeof(gdouble), 1, file);
    return caisse;
}


int category_set_price(FILE *file,  CategoryType catType, ClientType ctype, gdouble price){
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


gdouble category_get_price(FILE *file,  CategoryType catType,  ClientType ct){
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


Employee* employee_new(gchar *fname, gchar *cin, gchar *username, gchar *password, short isAdmin, short isActive){
    Employee *emp = (Employee*) malloc(sizeof(Employee));
    if (emp) {
        g_strlcpy(emp->fullname, fname, NAME_LEN-1);
        g_strlcpy(emp->cin, cin, CIN_LEN-1);
        g_strlcpy(emp->username, username, USERNAME_LEN-1);
        g_strlcpy(emp->password, password, PASSWORD_LEN-1);
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


Ticket* ticket_new(FILE* file, gdouble price, CategoryType cat, ClientType client){
    Ticket *tkt = (Ticket*) malloc(sizeof(Ticket));
    tkt->id = ticket_get_next_id(file);
    tkt->category.type = cat;
    tkt->category.client = client;
    tkt->category.price = price;
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
    g_signal_connect(btnSettings, "clicked",G_CALLBACK(settings_dialog_show), (gpointer) app); 



    app->mainWindow.window = window;

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

    gtk_widget_show_all(loginWindow);
}


void login(GtkWidget* widget, gpointer data){
    
    App *app = (App*) data;
    Employee *emp;
    gchar *username = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->loginWindow.txtUsername));
    gchar *password = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->loginWindow.txtPassword));

    if(employee_exist(app->usersFile,username)){
        emp = employee_get(app->usersFile,username);
        if (strcmp(emp->password,password) == 0)
        {
            if (emp->isActive)
            {
                app->logedUser = (*emp);
                log_action(app->logFile,username,-1,"connexion avec succès");
                if (emp->isAdmin)
                {
                    admin_window(app);
                }else{
                    // normal user
                    employee_window(app);
                }

                gtk_widget_hide(app->loginWindow.window);

            }else{
                log_action(app->logFile,username,-1,"échec de la connexion");
                GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(app->loginWindow.window),
                            GTK_DIALOG_MODAL,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_OK,
                            "Votre compte est desactivé veuillez contacter votre administrateur");
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
            }
            
            

        }else{
            log_action(app->logFile,username,-1,"échec de la connexion");
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

            log_action(app->logFile,username,-1,"échec de la connexion");
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
    g_signal_connect(btnAdd, "clicked",G_CALLBACK(users_subdialog_show), app); 
    g_signal_connect(btnEdit, "clicked",G_CALLBACK(users_subdialog_edit_show), app); 



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

void users_subdialog_add(App *app){
    GtkWidget   *window,
                *btnSave,
                *btnCancel,
                *grid,
                *lblUsername,
                *lblPassword,
                *lblFullName,
                *lblCIN,
                *lblIsAdmin,
                *lblIsActive,
                *switchIsAdmin,
                *switchIsActive,
                *entryUsername,
                *entryPassword,
                *entryCIN,
                *entryFullName;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window),"Ajouter un utilisateur");
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->usersDialog.window));

    grid = gtk_grid_new();

    btnSave = gtk_button_new_with_label("Ajouter");
    btnCancel = gtk_button_new_with_label("Annuler");

    lblFullName = gtk_label_new("Nom complet :");
    lblUsername = gtk_label_new("Nom de l'utilisateur :");
    lblPassword = gtk_label_new("Mot de passe :");
    lblCIN = gtk_label_new("CIN :");
    lblIsAdmin= gtk_label_new("Admin :");
    lblIsActive = gtk_label_new("Active :");
    

    switchIsActive = gtk_switch_new();
    switchIsAdmin = gtk_switch_new();

    entryUsername = gtk_entry_new();
    entryPassword = gtk_entry_new();
    entryFullName = gtk_entry_new();
    entryCIN      = gtk_entry_new();



    gtk_grid_attach(GTK_GRID(grid),lblFullName,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryFullName,1,0,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblUsername,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryUsername,1,1,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblPassword,0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryPassword,1,2,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblCIN,0,3,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryCIN,1,3,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblIsAdmin,0,4,1,1);
    gtk_grid_attach(GTK_GRID(grid),switchIsAdmin,1,4,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblIsActive,0,5,1,1);
    gtk_grid_attach(GTK_GRID(grid),switchIsActive,1,5,1,1);
    
    gtk_grid_attach(GTK_GRID(grid),btnSave,0,6,1,1);
    gtk_grid_attach(GTK_GRID(grid),btnCancel,1,6,1,1);


    g_signal_connect(G_OBJECT(btnSave),"clicked",G_CALLBACK(users_subdialog_add_save),app);
    g_signal_connect(G_OBJECT(btnCancel),"clicked",G_CALLBACK(users_subdialog_add_cancel),app);





    gtk_container_add(GTK_CONTAINER(window),grid);


    app->usersDialogAdd.window = window;
    app->usersDialogAdd.entryFullName = entryFullName;
    app->usersDialogAdd.entryUsername = entryUsername;
    app->usersDialogAdd.entryPassword = entryPassword;
    app->usersDialogAdd.entryCIN = entryCIN;
    app->usersDialogAdd.switchIsActive = switchIsActive;
    app->usersDialogAdd.switchIsAdmin = switchIsAdmin;




    gtk_widget_show_all(window);
}

void users_subdialog_show(GtkWidget* widget,gpointer data){

    users_subdialog_add((App*) data);
}

void users_subdialog_add_save(GtkWidget* widget,gpointer data){
    App *app = (App*) data;
    GtkTreeIter    iter;
    gchar *username,*fullname,*password,*cin;
    gboolean isActive,isAdmin;
    Employee *e;
    fullname = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogAdd.entryFullName));
    username = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogAdd.entryUsername));
    password = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogAdd.entryPassword));
    cin      = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogAdd.entryCIN));
    isActive = gtk_switch_get_active(GTK_SWITCH(app->usersDialogAdd.switchIsActive));
    isAdmin = gtk_switch_get_active(GTK_SWITCH(app->usersDialogAdd.switchIsAdmin));

    e = employee_new(fullname,cin,username,password,isAdmin,isActive);
    employee_add(app->usersFile,e);
    employee_free(e);
    gtk_list_store_append (app->usersDialog.store, &iter);
    gtk_list_store_set (app->usersDialog.store, &iter,
                      COL_FNAME, fullname,
                      COL_USERNAME, username,
                      COL_PASSWORD, password,
                      COL_CIN, cin,
                      COL_ACTIVE, isActive,
                      COL_ADMIN,isAdmin,
                      -1);
    log_action(app->logFile,app->logedUser.username,-1,"Ajout d'un utilisateur");
    gtk_window_close(GTK_WINDOW(app->usersDialogAdd.window));
}

void users_subdialog_add_cancel(GtkWidget *widget,gpointer data){
    App *app = (App*) data;
    gtk_window_close(GTK_WINDOW(app->usersDialogAdd.window));
}


void users_subdialog_edit(App *app){
    GtkWidget   *window,
                *btnSave,
                *btnCancel,
                *grid,
                *lblUsername,
                *lblPassword,
                *lblFullName,
                *lblCIN,
                *lblIsAdmin,
                *lblIsActive,
                *switchIsAdmin,
                *switchIsActive,
                *entryUsername,
                *entryPassword,
                *entryCIN,
                *entryFullName;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window),"Editer un utilisateur");
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->usersDialog.window));

    grid = gtk_grid_new();

    btnSave = gtk_button_new_with_label("Enregistrer");
    btnCancel = gtk_button_new_with_label("Annuler");

    lblFullName = gtk_label_new("Nom complet :");
    lblUsername = gtk_label_new("Nom de l'utilisateur :");
    lblPassword = gtk_label_new("Mot de passe :");
    lblCIN = gtk_label_new("CIN :");
    lblIsAdmin= gtk_label_new("Admin :");
    lblIsActive = gtk_label_new("Active :");
    

    switchIsActive = gtk_switch_new();
    switchIsAdmin = gtk_switch_new();

    entryUsername = gtk_entry_new();
    entryPassword = gtk_entry_new();
    entryFullName = gtk_entry_new();
    entryCIN      = gtk_entry_new();



    gtk_grid_attach(GTK_GRID(grid),lblFullName,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryFullName,1,0,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblUsername,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryUsername,1,1,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblPassword,0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryPassword,1,2,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblCIN,0,3,1,1);
    gtk_grid_attach(GTK_GRID(grid),entryCIN,1,3,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblIsAdmin,0,4,1,1);
    gtk_grid_attach(GTK_GRID(grid),switchIsAdmin,1,4,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblIsActive,0,5,1,1);
    gtk_grid_attach(GTK_GRID(grid),switchIsActive,1,5,1,1);
    
    gtk_grid_attach(GTK_GRID(grid),btnSave,0,6,1,1);
    gtk_grid_attach(GTK_GRID(grid),btnCancel,1,6,1,1);


    g_signal_connect(G_OBJECT(btnSave),"clicked",G_CALLBACK(users_subdialog_edit_save),app);
    g_signal_connect(G_OBJECT(btnCancel),"clicked",G_CALLBACK(users_subdialog_edit_cancel),app);





    gtk_container_add(GTK_CONTAINER(window),grid);


    app->usersDialogEdit.window = window;
    app->usersDialogEdit.entryFullName = entryFullName;
    app->usersDialogEdit.entryUsername = entryUsername;
    app->usersDialogEdit.entryPassword = entryPassword;
    app->usersDialogEdit.entryCIN = entryCIN;
    app->usersDialogEdit.switchIsActive = switchIsActive;
    app->usersDialogEdit.switchIsAdmin = switchIsAdmin;




    gtk_widget_show_all(window);
}

void users_subdialog_edit_show(GtkWidget *widget, gpointer data){
    App *app = (App*) data;
    GtkTreeIter    iter;
    GtkTreeModel *model;
    gchar *username,*fullname,*password,*cin;
    gboolean isActive,isAdmin;
    GtkTreeSelection * selection;

    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(app->usersDialog.treeView));
    app->usersDialogEdit.selection = selection;
    
    if (gtk_tree_selection_get_selected(selection,&model,&iter)){
            gtk_tree_model_get (model, &iter,
                                COL_USERNAME, &username,
                                COL_FNAME, &fullname,
                                COL_PASSWORD, &password,
                                COL_CIN,&cin,
                                COL_ACTIVE,&isActive,
                                COL_ADMIN,&isAdmin,
                                -1);

            users_subdialog_edit(app);
            gtk_entry_set_text(GTK_ENTRY(app->usersDialogEdit.entryFullName),fullname);
            gtk_entry_set_text(GTK_ENTRY(app->usersDialogEdit.entryUsername),username);
            gtk_entry_set_text(GTK_ENTRY(app->usersDialogEdit.entryPassword),password);
            gtk_entry_set_text(GTK_ENTRY(app->usersDialogEdit.entryCIN),cin);
            gtk_switch_set_active(GTK_SWITCH(app->usersDialogEdit.switchIsActive),isActive);
            gtk_switch_set_active(GTK_SWITCH(app->usersDialogEdit.switchIsAdmin),isAdmin);

            g_free(fullname);
            g_free(username);
            g_free(password);
            g_free(cin);
        }
    }
    
    

void users_subdialog_edit_save(GtkWidget* widget,gpointer data){
    App *app = (App*) data;
    GtkTreeIter    iter;
    GtkTreeModel *model;
    gchar *username,*fullname,*password,*cin;
    gboolean isActive,isAdmin;
    Employee *e;

    fullname = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogEdit.entryFullName));
    username = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogEdit.entryUsername));
    password = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogEdit.entryPassword));
    cin      = (gchar*) gtk_entry_get_text(GTK_ENTRY(app->usersDialogEdit.entryCIN));
    isActive = gtk_switch_get_active(GTK_SWITCH(app->usersDialogEdit.switchIsActive));
    isAdmin = gtk_switch_get_active(GTK_SWITCH(app->usersDialogEdit.switchIsAdmin));

    e = employee_new(fullname,cin,username,password,isAdmin,isActive);
    employee_edit(app->usersFile,username,e);
    employee_free(e);
    gtk_tree_selection_get_selected(app->usersDialogEdit.selection,&model,&iter);
    gtk_list_store_set (app->usersDialog.store, &iter,
                      COL_FNAME, fullname,
                      COL_USERNAME, username,
                      COL_PASSWORD, password,
                      COL_CIN, cin,
                      COL_ACTIVE, isActive,
                      COL_ADMIN,isAdmin,
                      -1);
    log_action(app->logFile,app->logedUser.username,-1,"Modification d'un utilisateur");
    gtk_window_close(GTK_WINDOW(app->usersDialogEdit.window));
}

void users_subdialog_edit_cancel(GtkWidget *widget,gpointer data){
    App *app = (App*) data;
    gtk_window_close(GTK_WINDOW(app->usersDialogAdd.window));
}

void settings_dialog(App* app){
    GtkWidget   *window,
                *grid,
                *lblCat1,
                *lblCat2,
                *lblCat3,
                *lblSubscriber,
                *lblNormal,
                *spinCat1Normal,
                *spinCat2Normal,
                *spinCat3Normal,
                *spinCat1Subscriber,
                *spinCat2Subscriber,
                *spinCat3Subscriber,
                *btnSave,
                *btnCancel;


    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window),"Paramètres");
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->mainWindow.window));
    gtk_container_set_border_width(GTK_CONTAINER(window),10);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid),10);
    gtk_grid_set_column_spacing(GTK_GRID(grid),10);

    btnSave = gtk_button_new_with_label("Enregistrer");
    btnCancel = gtk_button_new_with_label("Annuler");

    lblCat1 = gtk_label_new("Prix (Categorie 1)");
    lblCat2 = gtk_label_new("Prix (Categorie 2)");
    lblCat3 = gtk_label_new("Prix (Categorie 3)");
    lblSubscriber = gtk_label_new("Client Abonné");
    lblNormal = gtk_label_new("Client Normale");

    spinCat1Normal = gtk_spin_button_new_with_range(0.0,1000.0,0.5);
    spinCat2Normal = gtk_spin_button_new_with_range(0.0,1000.0,0.5);
    spinCat3Normal = gtk_spin_button_new_with_range(0.0,1000.0,0.5);
    spinCat1Subscriber = gtk_spin_button_new_with_range(0.0,1000.0,0.5);
    spinCat2Subscriber = gtk_spin_button_new_with_range(0.0,1000.0,0.5);
    spinCat3Subscriber = gtk_spin_button_new_with_range(0.0,1000.0,0.5);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCat1Normal) ,
                             category_get_price(app->configFile,CAT1,NORMAL)
                             );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCat2Normal) ,
                             category_get_price(app->configFile,CAT2,NORMAL)
                             );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCat3Normal) ,
                             category_get_price(app->configFile,CAT3,NORMAL)
                             );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCat1Subscriber) ,
                             category_get_price(app->configFile,CAT1,SUBSCRIBER)
                             );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCat2Subscriber) ,
                             category_get_price(app->configFile,CAT2,SUBSCRIBER)
                             );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCat3Subscriber) ,
                             category_get_price(app->configFile,CAT3,SUBSCRIBER)
                             );




    gtk_grid_attach(GTK_GRID(grid),lblNormal,1,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),lblSubscriber,2,0,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblCat1,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCat1Normal,1,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCat1Subscriber,2,1,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblCat2,0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCat2Normal,1,2,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCat2Subscriber,2,2,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblCat3,0,3,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCat3Normal,1,3,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCat3Subscriber,2,3,1,1);
    
    gtk_grid_attach(GTK_GRID(grid),btnSave,1,4,1,1);
    gtk_grid_attach(GTK_GRID(grid),btnCancel,2,4,1,1);


    g_signal_connect(G_OBJECT(btnSave),"clicked",G_CALLBACK(settings_dialog_save),app);
    g_signal_connect(G_OBJECT(btnCancel),"clicked",G_CALLBACK(settings_dialog_cancel),app);





    gtk_container_add(GTK_CONTAINER(window),grid);


    app->settingsDialog.window = window;
    app->settingsDialog.spinCat1Normal = spinCat1Normal;
    app->settingsDialog.spinCat2Normal = spinCat2Normal;
    app->settingsDialog.spinCat3Normal = spinCat3Normal;
    app->settingsDialog.spinCat1Subscriber = spinCat1Subscriber;
    app->settingsDialog.spinCat2Subscriber = spinCat2Subscriber;
    app->settingsDialog.spinCat3Subscriber = spinCat3Subscriber;




    gtk_widget_show_all(window);
}

void settings_dialog_show(GtkWidget *widget,gpointer data){


    settings_dialog((App*) data);
}

void settings_dialog_save(GtkWidget *widget,gpointer data){
    App *app = (App*) data;

    category_set_price(app->configFile,CAT1,NORMAL,
       gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->settingsDialog.spinCat1Normal)));

    category_set_price(app->configFile,CAT2,NORMAL,
       gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->settingsDialog.spinCat2Normal)));
    category_set_price(app->configFile,CAT3,NORMAL,
       gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->settingsDialog.spinCat3Normal)));

    category_set_price(app->configFile,CAT1,SUBSCRIBER,
       gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->settingsDialog.spinCat1Subscriber)));
    category_set_price(app->configFile,CAT2,SUBSCRIBER,
       gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->settingsDialog.spinCat2Subscriber)));
    category_set_price(app->configFile,CAT3,SUBSCRIBER,
       gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->settingsDialog.spinCat3Subscriber)));
    
    log_action(app->logFile,app->logedUser.username,-1,"Changement des Paramètres");
    settings_dialog_cancel(NULL,app);
}

void settings_dialog_cancel(GtkWidget *widget,gpointer data){
    App *app = (App*) data;
    gtk_window_close(GTK_WINDOW(app->settingsDialog.window));
}


void employee_window(App *app){
    GtkWidget   *window,
                *image,
                *vbox,
                *btnExit,
                *btnCheckOut,
                *btnTicket;


    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window),900,600);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

    btnTicket = gtk_button_new();
    image = gtk_image_new_from_file("img/ticket.png");
    gtk_button_set_image(GTK_BUTTON(btnTicket),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnTicket,TRUE,FALSE,0);

    btnCheckOut = gtk_button_new();
    image = gtk_image_new_from_file("img/checkout.png");
    gtk_button_set_image(GTK_BUTTON(btnCheckOut),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnCheckOut,TRUE,FALSE,0);



    btnExit = gtk_button_new();
    image = gtk_image_new_from_file("img/exit.png");
    gtk_button_set_image(GTK_BUTTON(btnExit),image);
    gtk_box_pack_start(GTK_BOX(vbox),btnExit,TRUE,FALSE,20);

    gtk_container_add(GTK_CONTAINER (window), vbox);

    g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL); 
    g_signal_connect(btnExit, "clicked",G_CALLBACK(gtk_main_quit), NULL); 
    g_signal_connect(btnTicket, "clicked",G_CALLBACK(ticket_dialog_show), (gpointer) app); 
    g_signal_connect(btnCheckOut, "clicked",G_CALLBACK(checkout_dialog_show), (gpointer) app); 


    app->employeeWindow.window = window;
    gtk_widget_show_all(window);    
}

void ticket_dialog(App *app){
    GtkWidget   *window,
                *grid,
                *radioSubscriber,
                *radioNormal,
                *comboCategory,
                *lblCategory,
                *lblClientType,
                *btnOK,
                *btnCancel;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window),"Retirer un ticket");
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->employeeWindow.window));
    gtk_container_set_border_width(GTK_CONTAINER(window),10);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid),10);
    gtk_grid_set_column_spacing(GTK_GRID(grid),10);

    btnOK = gtk_button_new_with_label("Retirer");
    btnCancel = gtk_button_new_with_label("Annuler");

    lblCategory = gtk_label_new("Categorie");
    lblClientType = gtk_label_new("Type de client");

    radioNormal = gtk_radio_button_new_with_label(NULL,"Client normale");
    GSList *group = gtk_radio_button_get_group (GTK_RADIO_BUTTON(radioNormal));
    radioSubscriber = gtk_radio_button_new_with_label(group,"Client Abonné");

    comboCategory = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboCategory),"Categorie 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboCategory),"Categorie 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboCategory),"Categorie 3");
    gtk_combo_box_set_active(GTK_COMBO_BOX(comboCategory),0);


    gtk_grid_attach(GTK_GRID(grid),lblCategory,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),comboCategory,1,0,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblClientType,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),radioNormal,1,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),radioSubscriber,1,2,1,1);
    
    gtk_grid_attach(GTK_GRID(grid),btnOK,0,3,1,1);
    gtk_grid_attach(GTK_GRID(grid),btnCancel,1,3,1,1);


    g_signal_connect(G_OBJECT(btnOK),"clicked",G_CALLBACK(ticket_dialog_ok),app);
    g_signal_connect(G_OBJECT(btnCancel),"clicked",G_CALLBACK(ticket_dialog_cancel),app);





    gtk_container_add(GTK_CONTAINER(window),grid);


    app->ticketDialog.window = window;
    app->ticketDialog.radioNormal = radioNormal;
    app->ticketDialog.radioSubscriber = radioSubscriber;
    app->ticketDialog.comboCategory = comboCategory;

    gtk_widget_show_all(window);


}

void ticket_dialog_show(GtkWidget *widget, gpointer data){

    ticket_dialog((App*) data);
}

void ticket_dialog_ok(GtkWidget* widget, gpointer data){
    App *app=(App*) data;
    CategoryType cat;
    ClientType ct;
    Ticket *tkt;
    cat= gtk_combo_box_get_active(GTK_COMBO_BOX(app->ticketDialog.comboCategory));
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticketDialog.radioNormal)))
    {
        ct = NORMAL;
    }else{
        ct = SUBSCRIBER;
    }
    gdouble catPrice = category_get_price(app->configFile,cat,ct);
    tkt = ticket_new(app->configFile,catPrice,cat,ct);
    caisse_add(app->caisseFile,catPrice);
    log_action(app->logFile,app->logedUser.username,tkt->id,"Retirer un Ticket");
    ticket_free(tkt);

    gtk_window_close(GTK_WINDOW(app->ticketDialog.window));

}

void ticket_dialog_cancel(GtkWidget *widget, gpointer data){

    gtk_window_close(GTK_WINDOW( ((App*) data)->ticketDialog.window ));
}


void checkout_dialog(App *app){
    GtkWidget   *window,
                *grid,
                *spinCurrentPrice,
                *lblCurrentPrice,
                *lblCaisse,
                *lblPrice,
                *btnOK,
                *btnCancel;
    gdouble currentCaissePrice=0.0;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window),"encaisser");
    gtk_window_set_modal(GTK_WINDOW(window),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(app->employeeWindow.window));
    gtk_container_set_border_width(GTK_CONTAINER(window),10);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid),10);
    gtk_grid_set_column_spacing(GTK_GRID(grid),10);

    btnOK = gtk_button_new_with_label("Ok");
    btnCancel = gtk_button_new_with_label("Annuler");


    currentCaissePrice = caisse_get(app->caisseFile);
    gchar *strPrice = g_strdup_printf("%.2lf",currentCaissePrice);
    lblCurrentPrice = gtk_label_new(strPrice);
    g_free(strPrice);

    lblPrice = gtk_label_new("Prix à encaisser :");
    lblCaisse = gtk_label_new("Montant à la caisse :");

    spinCurrentPrice = gtk_spin_button_new_with_range(0.0,1000000.0,0.5);


    gtk_grid_attach(GTK_GRID(grid),lblCaisse,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),lblCurrentPrice,1,0,1,1);

    gtk_grid_attach(GTK_GRID(grid),lblPrice,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),spinCurrentPrice,1,1,1,1);    
    gtk_grid_attach(GTK_GRID(grid),btnOK,0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid),btnCancel,1,2,1,1);


    g_signal_connect(G_OBJECT(btnOK),"clicked",G_CALLBACK(checkout_dialog_ok),app);
    g_signal_connect(G_OBJECT(btnCancel),"clicked",G_CALLBACK(checkout_dialog_cancel),app);





    gtk_container_add(GTK_CONTAINER(window),grid);


    app->checkOutDialog.window = window;
    app->checkOutDialog.spinCurrentPrice = spinCurrentPrice;

    gtk_widget_show_all(window);

}


void checkout_dialog_show(GtkWidget* widget, gpointer* data){

    checkout_dialog((App*) data);
}
void checkout_dialog_ok(GtkWidget* widget, gpointer* data){
    App *app = (App*) data;
    gdouble caissePrice = caisse_get(app->caisseFile);
    gdouble priceToCheckOut = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->checkOutDialog.spinCurrentPrice));
    if ( caissePrice < priceToCheckOut)
    {
        GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(app->checkOutDialog.window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Le montant à la caisse est insuffisant");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }else{
        caisse_sub(app->caisseFile,priceToCheckOut);
        log_action(app->logFile,app->logedUser.username,-1,"Encaissement");
        checkout_dialog_cancel(NULL,(gpointer) app);
    }
}

void checkout_dialog_cancel(GtkWidget* widget, gpointer* data) {
    App *app = (App*) data;
    gtk_window_close(GTK_WINDOW(app->checkOutDialog.window));
}
















