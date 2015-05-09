/*************************************************
 * server.c                                    		  *
 *   create the server window    		  *
 *   send message to client          		  *
 *************************************************/
#include "server.h"
#include<gtk/gtk.h>
bool socket_start;
GtkTextBuffer *show_buffer,*input_buffer;
void quit_win(GtkWidget *,gpointer);
int main(int argc,char **argv)
{
	GtkWidget *window;
	GtkWidget *show_text,*input_text;
	GtkWidget *send_button,*quit_button,*cls_button,*start_button;
	GtkWidget *hbox,*vbox;
	GtkWidget *scrolled1,*scrolled2;
	GtkWidget *space_label;
	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Server");
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window),430,320);
	/* "quit" button */
	g_signal_connect(GTK_OBJECT(window),"destroy",GTK_SIGNAL_FUNC(quit_win),NULL);
	
	space_label=gtk_label_new("                          ");
	/* set button */
	send_button=gtk_button_new_with_label("Send");
	quit_button=gtk_button_new_with_label("Close");
	cls_button=gtk_button_new_with_label("Clear");
	start_button=gtk_button_new_with_label("Startup");
	/* set textbox */
	show_text=gtk_text_view_new();
	input_text=gtk_text_view_new();
	/* get the buffer of textbox */
	show_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(show_text));
	input_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(input_text));
	/* set textbox to diseditable */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(show_text),FALSE);
	/* scroll window */
	scrolled1=gtk_scrolled_window_new(NULL,NULL);
	scrolled2=gtk_scrolled_window_new(NULL,NULL);
	/* create a textbox */
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled1),show_text);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled2),input_text);
	/* setting of window */
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled1),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	hbox=gtk_hbox_new(FALSE,2);
	vbox=gtk_vbox_new(FALSE,2);
	/* click quit to call quit_win*/
	g_signal_connect(GTK_OBJECT(quit_button),"clicked",GTK_SIGNAL_FUNC(quit_win),NULL);
	/* click to clear input screen*/
	g_signal_connect(GTK_OBJECT(cls_button),"clicked",GTK_SIGNAL_FUNC(clean_send_text),NULL);
	/* click Start to build socket*/
	g_signal_connect(GTK_OBJECT(start_button),"clicked",GTK_SIGNAL_FUNC(startup),NULL);
	/* create window */
	gtk_box_pack_start(GTK_BOX(hbox),start_button,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),space_label,TRUE,TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),send_button,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),cls_button,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),quit_button,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(vbox),scrolled1,TRUE,TRUE,2);
	gtk_box_pack_start(GTK_BOX(vbox),scrolled2,TRUE,TRUE,2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,2);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	/* click send button ,then call send_text*/
	gtk_signal_connect(GTK_OBJECT(send_button),"clicked",GTK_SIGNAL_FUNC(send_text),NULL);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
/* show the input text */
void show_local_text(const gchar* text)
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(show_buffer),&start,&end);/*获得缓冲区开始和结束位置的Iter*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,"Me:\n",4);/*插入文本到缓冲区*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,text,strlen(text));/*插入文本到缓冲区*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,"\n",1);/*插入文本到缓冲区*/
}
/* clean the input text */
void clean_send_text()
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(input_buffer),&start,&end);/*获得缓冲区开始和结束位置的Iter*/
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(input_buffer),&start,&end);/*插入到缓冲区*/
}
/* get the input text,and send it */
void send_text()
{
	GtkTextIter start,end;
	gchar *text;
	 /* Socket creating has succeed ,so send message */
	text=(gchar *)malloc(MAXSIZE);
	if(text==NULL)
	{
		printf("Malloc error!\n");
		exit(1);
	}
	/* get text */
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(input_buffer),&start,&end);
	text=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(input_buffer),&start,&end,FALSE);
	/* If there is no input,do nothing but return */
	if(strcmp(text,"")!=0)
	{
		send_func(text);
		clean_send_text();
		show_local_text(text);
	}
	else
		show_err("The message can not be empty ...\n");
	free(text);
}
void startup(void)
{
//	if( !socket_start )
//	{
//		build_socket();
//		socket_start=true;
//		return;
//	}
//	show_err("The server has been started !\n");

        int res;     
        pthread_t listen_thread;
        res=pthread_create(&listen_thread,NULL,build_socket,NULL); 
	if(res!=0)
	{
            show_err("Thread create error");
            exit(EXIT_FAILURE);
	}
	show_err("The server has been started !\n");
        return;
}
/* show errors such as "no input","haven't create sockt" etc. */
void show_err(char *err)
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(show_buffer),&start,&end);/*获得缓冲区开始和结束位置的Iter*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,err,strlen(err));
}

/* show the received message */
void show_remote_text(char rcvd_mess[])
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(show_buffer),&start,&end);/*获得缓冲区开始和结束位置的Iter*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,"Client:\n",8);/*插入文本到缓冲区*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,rcvd_mess,strlen(rcvd_mess));/*插入文本到缓冲区*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(show_buffer),&end,"\n",1);/*插入换行到缓冲区*/
}
/* quit */
void quit_win(GtkWidget *window,gpointer data)
{
	gtk_main_quit();
}
