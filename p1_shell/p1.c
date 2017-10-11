#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define TRUE 1


typedef struct ccc{
	int pipe;
	int c1_out;
	int c1_in;
	char *c1_infile;
	char *c1_outfile;
	int c2_out;
	int c2_in;
	char *c2_infile;
	char *c2_outfile;
}com_info;

int read_command(char* para1[],char* para2[],com_info *cinfo);

int main()
{
	int status;
	int sfd=dup(STDOUT_FILENO);
	int sfin=dup(STDIN_FILENO);
    while(TRUE){
    	/*command memory preparation*/
   		char * para1[20];
   		char * para2[20];
		int j;
		for(j=0;j<20;j++){
			para1[j]=(char*)malloc(25*sizeof(char));
			para2[j]=(char*)malloc(25*sizeof(char));
			memset(para1[j],0,sizeof(para1[j]));
			memset(para2[j],0,sizeof(para2[j]));
		} 
		com_info new={0,0,0,"","",0,0,"",""};
		com_info *cinfo=&new;
		cinfo->c1_outfile=(char*)malloc(10*sizeof(char));
		cinfo->c1_infile=(char*)malloc(10*sizeof(char));
		cinfo->c2_outfile=(char*)malloc(10*sizeof(char));
		cinfo->c2_infile=(char*)malloc(10*sizeof(char));
		memset(cinfo->c1_infile,0,sizeof(cinfo->c1_infile));
		memset(cinfo->c1_outfile,0,sizeof(cinfo->c1_outfile));
		memset(cinfo->c2_infile,0,sizeof(cinfo->c2_infile));
		memset(cinfo->c2_outfile,0,sizeof(cinfo->c2_outfile));
		dup2(sfd,STDOUT_FILENO);
		dup2(sfin,STDIN_FILENO);
    	/*read_command*/
    	printf("ve482sh $"); 
    	int abbb=read_command(para1,para2,cinfo);
    	if(abbb==0) continue;
    	/*catch input error*/
    	if((cinfo->c1_in==1)&&!strcmp(cinfo->c1_infile,"\0")){
    		perror("input file incomplete");
    		continue;
    	}

    	/*if exit*/
    	char* tmp="exit";
    	char* tmp2="^d";
    	if(strcmp(para1[0],tmp)==0||strcmp(para1[0],tmp2)==0){
    		for(j=0;j<20;j++){
				free(para1[j]);
				free(para2[j]);
			} 
			free(cinfo->c1_outfile);
			free(cinfo->c1_infile);
			free(cinfo->c2_outfile);
			free(cinfo->c2_infile);
			exit(0);
			break;
    	}

    	/*if cd*/
    	char * cd="cd";
    	if(strcmp(para1[0],"cd")==0){
    		if(para1[1][0]!='~'){
    			char buff[80];
    			getcwd(buff, sizeof(buff));
    			if(chdir(para1[1])<0){
    				char *full_dir=(char *)malloc(100*sizeof(char));
    				char new[40];
    				memset(new,0,sizeof(new));
    				if((para1[1][0]=='.')&&(para1[1][1]!='.')){
    					strcpy(full_dir,buff);
    					memcpy(new,para1[1]+1, (strlen(para1[1])-1)*sizeof(char));
    					strcat(full_dir,new);
    					perror(full_dir);
    				}
    				else if((para1[1][0]=='.')&&(para1[1][1]=='.')){
    					int number=strlen(buff);
    					int i=number-1;
    					while(buff[i]!='/'){
    						i--;
    					}
    					memcpy(new,buff,i*sizeof(char));
    					strcpy(full_dir,new);
    					memset(new,0,sizeof(new));
    					memcpy(new,para1[1]+2, (strlen(para1[1])-2)*sizeof(char));
    					strcat(full_dir,new);
    					perror(full_dir);
    				}
    				else{
    					strcpy(full_dir,buff);
    					strcat(full_dir,"/");
    					strcat(full_dir,para1[1]);
    					perror(full_dir);
    					perror(full_dir);
    				}
    			}	
    		}
    		else{
    			char *home="/home/parallels";
    			char *full_dir=(char*)malloc(80*sizeof(char));
    			memset(full_dir,0,sizeof(full_dir));
    			int num=strlen(para1[1]);
    			char newdir[40];
    			memset(newdir,0,sizeof(newdir));
    			memcpy(newdir,para1[1]+1, (num-1)*sizeof(char));
    			strcpy(full_dir,home);
    			strcat(full_dir, newdir);
    			if(chdir(full_dir)<0){
    				perror(full_dir);
    			}
    			free(full_dir);
    		}
    		for(j=0;j<20;j++){
				free(para1[j]);
				free(para2[j]);
			} 
    		continue;
    	}

    	/*execute command*/  
    	pid_t fpid=fork();
    	if(fpid<0){
    		printf("error in fork!\n");
    	}
    	else if(fpid!=0){/*parent code*/
    		waitpid(-1,&status,0); 
    	}
    	else{/*child code*/
    		if(cinfo->pipe==0)
    		{
    			if(cinfo->c1_out>0){/*output redirect*/
    				int fd=-1;
    				if(cinfo->c1_out==1){/*cover orginal*/
    			   		fd=open(cinfo->c1_outfile,O_CREAT | O_RDWR | O_TRUNC);
    				}
    				else if(cinfo->c1_out==2){/*append*/
    			   		fd=open(cinfo->c1_outfile,O_CREAT | O_RDWR | O_APPEND);
    				}
					if(fd<0){
						perror("open error");
					}
					else{
						dup2(fd,STDOUT_FILENO);
						close(fd);
    					execvp(para1[0],para1);
					}
					dup2(sfd,STDOUT_FILENO);
  				}
  				if(cinfo->c1_in==1){/*input redirect*/
    				int fd=-1;
    			   	fd=open(cinfo->c1_infile,O_RDONLY);
					if(fd<0){
						perror("open error");
					}
					else{
						dup2(fd,STDIN_FILENO);
						close(fd);
    					execvp(para1[0],para1);
					}
					dup2(sfin,STDIN_FILENO);
  				}
  				else{
    				execvp(para1[0],para1);
    			}	
    		}
    		else if(cinfo->pipe==1){
    			int fd[2];
    			pipe(fd);
    			pid_t  subchild=fork();
    			if(subchild==-1){
    				printf("Error:fork");  
               		exit(1);  
    			}
    			else if(subchild==0){
              		close(fd[0]); /*close read*/
              		dup2(fd[1],1);
             		close(fd[1]);
             		if(cinfo->c1_in==1){
    					int ff=-1;
    			   		ff=open(cinfo->c1_infile,O_RDONLY);
						if(ff<0){
							perror("open error");
						}
						else{
							dup2(ff,0);
							close(ff);
    						execvp(para1[0],para1);
						}
						dup2(sfin,0);
  					}
  					else{
    					execvp(para1[0],para1);
    				}	
              		dup2(sfd,1);
              		exit(0);  
    			}
    			else{
    				while(waitpid(-1,&status,0)!=subchild);
              		dup2(sfd,1);
              		close(fd[1]); /*close write*/
              		dup2(fd[0],0);
             		close(fd[0]);
              		if(cinfo->c2_out==0){
              			execvp(para2[0],para2); 
              		}
              		else{
              			int f=-1;
    					if(cinfo->c2_out==1){/*cover orginal*/
    			   			f=open(cinfo->c2_outfile,O_CREAT | O_RDWR | O_TRUNC);
    					}
    					else if(cinfo->c2_out==2){/*append*/
    			   			f=open(cinfo->c2_outfile,O_CREAT | O_RDWR | O_APPEND);
    					}
						if(f<0){
							perror("open error");
						}
						else{
							dup2(f,STDOUT_FILENO);
							close(f);
    						execvp(para2[0],para2);
						}
						dup2(sfd,STDOUT_FILENO);
              		}
              		
    			}

    		}
    		
    	}
    	/*free memory*/
    	for(j=0;j<20;j++){
			free(para1[j]);
			free(para2[j]);
		} 
		free(cinfo->c1_outfile);
		free(cinfo->c1_infile);
		free(cinfo->c2_outfile);
		free(cinfo->c2_infile);
    }
    return 0;
}









int  read_command(char* para1[],char* para2[],com_info *cinfo){ 
	int j=0;
	char *s=(char*)malloc(1024*sizeof(char));
	int begin=1;
	int end=0;
	int para_num1=0;
	int para_num2=0;
	int file_reading=0;
	int quote=0;
	char * ttt=fgets(s,1025,stdin);
	if(!strcmp(s,"\n")) return 0;
	if(ttt==NULL){
		return 0;
	}
	else{
		int i=0;
		for(i=0;i<strlen(s);i++){
			/*quote*/
			if((s[i]=='\"')&& (quote==0)){
				quote=1; 
				begin++;
				end++;
				continue;
			}
			else if((s[i]=='\"')&&(quote==1)){
				quote=0;
				if(cinfo->pipe==1){
						memcpy(para2[para_num2],s+begin-1, (end-begin+1)*sizeof(char));
						para_num2=para_num2+1;
						end=end+1;
				    	begin=end+1;
				}
				else if(cinfo->pipe==0){
						memcpy(para1[para_num1],s+begin-1, (end-begin+1)*sizeof(char));
						para_num1=para_num1+1;
						end=end+1;
				    	begin=end+1;
				}
				continue;
			}
			else if((s[i]!='\"')&&(quote==1)){
				end=end+1;
				continue;
			}
			/*common command*/
			if((s[i]==' ')||(s[i]=='>')||(s[i]=='<')||(s[i]=='\n')||(s[i]=='|')){
				/*no parameter to copy*/
				if(begin==end+1){
				 	begin++;
				 	end++;
				}/*copy last parameter or copy file_name*/
				else{
				 	if(file_reading==1){
				 		if(cinfo->pipe==1){
				 			if(cinfo->c2_in==1){
				 				memcpy(cinfo->c2_infile,s+begin-1, (end-begin+1)*sizeof(char));
								end=end+1;
								begin=end+1;
								file_reading=0;
				 			}
				 			else{
				 				memcpy(cinfo->c2_outfile,s+begin-1, (end-begin+1)*sizeof(char));
								end=end+1;
								begin=end+1;
								file_reading=0;
				 			}
				 		}
				 		else{
				 			if(cinfo->c1_in==1){
				 				memcpy(cinfo->c1_infile,s+begin-1, (end-begin+1)*sizeof(char));
								end=end+1;
								begin=end+1;
								file_reading=0;
				 			}
				 			else{
				 				memcpy(cinfo->c1_outfile,s+begin-1, (end-begin+1)*sizeof(char));
								end=end+1;
								begin=end+1;
								file_reading=0;
				 			}
				 		}
				 	}
				 	else if(cinfo->pipe==1){
				 		memcpy(para2[para_num2],s+begin-1, (end-begin+1)*sizeof(char));
						para_num2=para_num2+1;
						end=end+1;
				    	begin=end+1;
				 	}
				 	else{
				 		memcpy(para1[para_num1],s+begin-1, (end-begin+1)*sizeof(char));
						para_num1=para_num1+1;
						end=end+1;
				    	begin=end+1;
				 	}
				}

				/*considering >, >>, < , |*/
				if((s[i]=='>')&&(s[i+1]!='>')){
				 	if(s[i-1]=='>') continue;
					if(cinfo->pipe==1){
						cinfo->c2_out=1;
						file_reading=1;
					}
					else{
						cinfo->c1_out=1;
						file_reading=1;
					}
				 }
				else if((s[i]=='>')&&(s[i+1]=='>')){
					if(cinfo->pipe==1){
						cinfo->c2_out=2;
						file_reading=1;
					}
					else{
						cinfo->c1_out=2;
						file_reading=1;
					}
				}
				else if(s[i]=='<'){
					if(cinfo->pipe==1){
						cinfo->c2_in=1;
						file_reading=1;
					}
					else{
						cinfo->c1_in=1;
						file_reading=1;
					}
				}
				else if(s[i]=='|'){
					cinfo->pipe=1;
					para1[para_num1]=(char *) 0;
				} 
				else if(s[i]=='\n'){
					if(cinfo->pipe==0) para1[para_num1]=(char *) 0;
					else if(cinfo->pipe==1) para2[para_num2]=(char *) 0;
					break;
				} 
			}
			else{
				end=end+1;
			}
		}
	}
	/*wait for command*/
	if((cinfo->c1_out>0) && !strcmp(cinfo->c1_outfile,"\0")){
		while(!strcmp(cinfo->c1_outfile,"\0")){
			printf(">");
			char *sss=(char *)malloc(100*sizeof(char));
			char * tat=fgets(sss,100,stdin);
			int pa=0;
			if(!strcmp(sss,"\n")){
				continue;
			}
			while(sss[pa]!='\n'){
				pa++;
			}
			char input[30];
			memset(input,0,sizeof(input));
			memcpy(input,sss,pa*sizeof(char));
			strcpy(cinfo->c1_outfile,input);
			free(sss);
		}
	}
	if( cinfo->pipe && s[end-2]=='|'){
		int pa=0;
		while(!pa){
			printf(">");
			char *ss=(char *)malloc(100*sizeof(char));
			char * taat=fgets(ss,100,stdin);
			pa=0;
			if(!strcmp(ss,"\n")){
				continue;
			}
			int beg=1;
			int en=0;
			while(ss[en]!='\n'){
				while((ss[en]!=' ')&& (ss[en]!='\n')){
					en++;
				}
				para2[pa]=(char *)malloc(25*sizeof(char));
				memset(para2[pa],0,sizeof(para2[pa]));
				memcpy(para2[pa],ss+beg-1,(en-beg+1)*sizeof(char));
				pa=pa+1;
				if(ss[en]=='\n') break;
				en++;
				beg=en+1;
			}
			free(ss);
		}
		para_num2=pa;
		int i=0;
		for(i=pa;i<20;i++){
			memset(para2[i],0,sizeof(para2[i]));
		}
	}
	/*printf("para2: %s,%s\n",para2[0],para2[1]);
	j=0;
	printf("pipe: %d\n", cinfo->pipe);
	printf("c1: %d %d\n", cinfo->c1_in, cinfo->c1_out);
	printf("c2: %d %d\n", cinfo->c2_in, cinfo->c2_out);
	printf("para1: \n");
	for(j=0;j<para_num1;j++){
		printf("%sm\n",para1[j]);
	}
	printf("c1_in: %s\n",cinfo->c1_infile);
	printf("c1_out: %s\n",cinfo->c1_outfile);
	printf("para2: \n");
	for(j=0;j<para_num2;j++){
		printf("%sm\n",para2[j]);
	}
	printf("c2_in: %s\n",cinfo->c2_infile);
	printf("c2_out: %s\n",cinfo->c2_outfile);*/
	return 1;
}