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

typedef struct {
     CategoryType type;
     ClientType client;
    float price;
} Category;


typedef struct {
    int id;
    double price;
    Category category;
    char date[30];
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
    char username[USERNAME_LEN];// user login name
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


void employee_init(void);

//------------- Ticket --------------//


Ticket* ticket_new( double price, Category cat,char* date, ClientType client);

void ticket_free(Ticket * tkt);



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
    MainWindow mainWindow;
    LoginWindow loginWindow;
    FILE* configFile;
    FILE* usersFile;
    FILE* caisseFile;
    FILE* logFile;
} App;


void app_init(App*);

void app_quit(App*);



void admin_window(App *app);

void login_window(App *app);

void login(GtkWidget* widget, gpointer data);


int main(int argc, char  *argv[])
{
    App app;

    //employee_init();

    gtk_init(&argc, &argv);

    app_init(&app);


    login_window(&app);
  
    gtk_widget_show_all(app.loginWindow.window); 

    gtk_main();
    app_quit(&app);


    return 0;
}



char* date_time_get(){
    char *str = (char*) malloc(sizeof(char)*30);
    time_t t;
    time(&t);
    strftime(str, 30, "%H:%M:%S %d/%m/%Y", localtime(&t));
    return str;
}


int log_action(FILE *file, Employee *emp,Ticket* tkt,char *msg){
    
    char *time = date_time_get();
    
    fprintf(file, "%s : user(%s) : ticket(%d) : %s \n",
            time,
            emp->username,
            tkt->id,
            msg);
    
    if(time){
        free(time);
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


Employee* employee_new(char *fname, char *cin, char *username, char *password, short isAdmin, short isActive){
    Employee *emp = (Employee*) malloc(sizeof(Employee));
    if (emp) {
        strncpy(emp->fullname, fname, NAME_LEN-1);
        strncpy(emp->cin, cin, CIN_LEN-1);
        strncpy(emp->username, username, USERNAME_LEN-1);
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


int employee_get_pos(FILE* file, char *username){
    int i;
    Employee e;
    fseek(file, 0, SEEK_SET);
    for (i=1 ; fread(&e, sizeof(Employee), 1, file) > 0 ; i++) {
        if (strcmp(e.username, username) == 0) {
            return i;
        }
    }
    return -1;
}


int employee_exist(FILE *file, char *username){

    return employee_get_pos(file, username) != -1;
}


Employee* employee_get(FILE *file, char *username){
    int pos = employee_get_pos(file, username);
    if (pos == -1) {
        return NULL;
    }
    Employee *emp = (Employee*) malloc(sizeof(Employee));
    fseek(file, sizeof(Employee) * (pos-1), SEEK_SET);
    fread(emp  , sizeof(Employee), 1, file);
    return emp;
}


int employee_edit(FILE *file, char *username,Employee *emp){
    int pos;
    if (employee_exist(file, username)) {
        pos = employee_get_pos(file, username);
        fseek(file, sizeof(Employee) * (pos-1), SEEK_SET);
        fwrite(emp, sizeof(Employee), 1, file);
    }
    return -1;
}


void employee_init(void){
    FILE* file = fopen(USERS_FILE,"w");
    if(!file){
        fprintf(stderr, "file not opened %s\n", USERS_FILE);
        return;
    }
    Employee *emp = employee_new("admin","AE12345678","admin","admin",1,1);
    employee_add(file,emp);
    employee_free(emp);

    emp = employee_new("adil","AE12845678","adil","adil",0,1);
    employee_add(file,emp);
    employee_free(emp);
    exit(0);
}



Ticket* ticket_new( double price, Category cat,char *date, ClientType client){
    Ticket *tkt = (Ticket*) malloc(sizeof(Ticket));
    tkt->price = price;
    tkt->category = cat;
    strncpy(tkt->date, date, 29);
    tkt->type = client;
    return tkt;
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
    gtk_window_fullscreen (GTK_WINDOW(window));

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
    app->logFile = fopen(LOG_FILE,"a");
    app->configFile = fopen(CONFIG_FILE,"r+");
    app->caisseFile = fopen(CAISSE_FILE,"r+");
}

void app_quit(App *app){
    fclose(app->configFile);
    fclose(app->usersFile);
    fclose(app->logFile);
    fclose(app->caisseFile);
}




























