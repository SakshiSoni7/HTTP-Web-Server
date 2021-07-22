#include<wp>
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
using namespace wp;
void Request::setKeyValue(string k,string v)
{
this->keyValues.insert(pair<string,string>(k,v));
}
string Request::getValue(string k)
{
map<string,string>::iterator iter;
iter=this->keyValues.find(k);
if(iter==this->keyValues.end()) return string("");
return iter->second;
}
string Request::get(string name)
{
string val;
int i,e;
for(i=0;i<this->dataCount;i++)
{
for(e=0;data[i][e]!='\0' && data[i][e]!='=';e++);
if(data[i][e]!='=') continue; 
if(strncmp(data[i],name.c_str(),e)==0) break;
}
if(i==this->dataCount)
{
val="";
}
else
{
val=string(data[i]+(e+1));
}
return val;
}
void Request::forward(string forwardTo)
{
this->forwardTo=forwardTo;
}
Response::Response(int clientSocketDescriptor)
{
this->clientSocketDescriptor=clientSocketDescriptor;
this->isClosed=false;
this->headerCreated=false;
}
void Response::createHeader()
{
char header[51];
strcpy(header,"HTTP/1.1 200 OK\nContent-Type:text/html\n\n");
send(this->clientSocketDescriptor,header,strlen(header),0);
this->headerCreated=true;
}
void Response::write(const char *str)
{
if(str==NULL) return;
int len=strlen(str);
if(len==0) return;
if(!this->headerCreated) createHeader();
send(this->clientSocketDescriptor,str,len,0);
}
void Response::write(string str)
{
write(str.c_str());
}
void Response::close()
{
if(this->isClosed) return;
closesocket(this->clientSocketDescriptor);
this->isClosed=true;
}
char** decode(char **c,int dc)
{
char **d;
int i,j,k;
d=(char **)malloc(sizeof(char*)*dc);
for(i=0;i<dc;i++)
{
d[i]=(char *)malloc(sizeof(char)*(strlen(c[i])));
for(k=0,j=0;j<strlen(c[i]);j++,k++)
{
if(c[i][j]=='+') d[i][k]=' ';
else if(c[i][j]=='%')
{
j++;
if(c[i][j]=='2')
{
j++;
if(c[i][j]=='0') d[i][k]=' ';
else if(c[i][j]=='1') d[i][k]='!';
else if(c[i][j]=='2') d[i][k]='"';
else if(c[i][j]=='3') d[i][k]='#';
else if(c[i][j]=='4') d[i][k]='$';
else if(c[i][j]=='5') d[i][k]='%';
else if(c[i][j]=='6') d[i][k]='&';
else if(c[i][j]=='7') d[i][k]='\'';
else if(c[i][j]=='8') d[i][k]='(';
else if(c[i][j]=='9') d[i][k]=')';
else if(c[i][j]=='B') d[i][k]='+';
else if(c[i][j]=='C') d[i][k]=',';
else if(c[i][j]=='F') d[i][k]='/';
}
else if(c[i][j]=='3')
{
j++;
if(c[i][j]=='A') d[i][k]=':';
else if(c[i][j]=='B') d[i][k]=';';
else if(c[i][j]=='C') d[i][k]='<';
else if(c[i][j]=='D') d[i][k]='=';
else if(c[i][j]=='E') d[i][k]='>';
else if(c[i][j]=='F') d[i][k]='?';
}
else if(c[i][j]=='4')
{
j++;
if(c[i][j]=='0') d[i][k]='@';
}
else if(c[i][j]=='5')
{
j++;
if(c[i][j]=='B') d[i][k]='[';
else if(c[i][j]=='C') d[i][k]='\\';
else if(c[i][j]=='D') d[i][k]=']';
else if(c[i][j]=='E') d[i][k]='^';
}
else if(c[i][j]=='6')
{
j++;
if(c[i][j]=='0') d[i][k]='`';
}
else if(c[i][j]=='7')
{
j++;
if(c[i][j]=='B') d[i][k]='{';
else if(c[i][j]=='C') d[i][k]='|';
else if(c[i][j]=='D') d[i][k]='}';
else if(c[i][j]=='E') d[i][k]='~';
}
}
else d[i][k]=c[i][j];
}
d[i][k]='\0';
free(c[i]);
}
free(c);
c=d;
}
int extensionEquals(const char *left,const char *right)
{
char a,b;
while(*left && *right)
{
a=*left;
b=*right;
if(a>=65 && a<=90) a+=32;
if(b>=65 && b<=90) b+=32;
if(a!=b) return 0;
left++;
right++;
}
return *left==*right;
}
char * getMIMEType(char *resource)
{
char *mimeType;
mimeType=NULL;
int len=strlen(resource);
if(len<4) return mimeType;
int lastIndexOfDot=len-1;
while(lastIndexOfDot>0 && resource[lastIndexOfDot]!='.') lastIndexOfDot--;
if(lastIndexOfDot==0) return mimeType;
if(extensionEquals(resource+lastIndexOfDot+1,"html"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"text/html");
}
if(extensionEquals(resource+lastIndexOfDot+1,"css"))
{
mimeType=(char *)malloc(sizeof(char)*9);
strcpy(mimeType,"text/css");
}
if(extensionEquals(resource+lastIndexOfDot+1,"js"))
{
mimeType=(char *)malloc(sizeof(char)*16);
strcpy(mimeType,"text/javascript");
}
if(extensionEquals(resource+lastIndexOfDot+1,"jpg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
if(extensionEquals(resource+lastIndexOfDot+1,"jpeg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
if(extensionEquals(resource+lastIndexOfDot+1,"png"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"image/png");
}
if(extensionEquals(resource+lastIndexOfDot+1,"ico"))
{
mimeType=(char *)malloc(sizeof(char)*13);
strcpy(mimeType,"image/x-icon");
}
return mimeType;
}
char isClientSideResource(char *resource)
{
int i;
for(i=0;resource[i]!='\0' && resource[i]!='.';i++);
if(resource[i]=='\0') return 'N';
return 'Y'; 
}
Request * parseRequest(char *bytes)
{
char method[11];
char resource[1001];
int i,j;
for(i=0;bytes[i]!=' ';i++) method[i]=bytes[i];
method[i]='\0';
i+=2;
char **data=NULL;
int dataCount=0;
if(strcmp(method,"GET")==0)
{
for(j=0;bytes[i]!=' ';j++,i++) 
{
if(bytes[i]=='?') break;
resource[j]=bytes[i];
}
resource[j]='\0';
if(bytes[i]=='?')
{
i++;
int si=i;
dataCount=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&') dataCount++;
i++;
}
dataCount++;
data=(char **)malloc(sizeof(char *)*dataCount);
int *pc=(int *)malloc(sizeof(int)*dataCount);
int j;
j=0;
i=si;
while(bytes[i]!=' ')
{
if(bytes[i]=='&') 
{
pc[j]=i;
j++;
}
i++;
}
pc[j]=i;
j=0;
i=si;
int howManyToPick;
while(j<dataCount)
{
howManyToPick=pc[j]-i;
data[j]=(char *)malloc(sizeof(char)*howManyToPick);
strncpy(data[j],bytes+i,howManyToPick);
data[j][howManyToPick]='\0';
i=pc[j]+1;
j++;
}
}
}
printf("Request arrived for %s\n",resource);
Request *request=new Request;
request->data=decode(data,dataCount);
request->dataCount=dataCount;
request->method=(char *)malloc((sizeof(char)*strlen(method))+1);
strcpy(request->method,method);
if(resource[0]=='\0')
{
request->resource=NULL;
request->isClientSideTechnologyResource='Y';
request->mimeType=NULL;
}
else
{
request->resource=(char *)malloc((sizeof(char)*strlen(resource))+1);
strcpy(request->resource,resource);
request->isClientSideTechnologyResource=isClientSideResource(resource);
request->mimeType=getMIMEType(resource);
}
return request;
}
WebProjector::WebProjector(int portNumber)
{
this->portNumber=portNumber;
}
WebProjector::~WebProjector()
{
if(this->requestMappings.size()!=0)
{
map<string,void (*)(Request &,Response &)>::iterator i;
for(i=this->requestMappings.begin();i!=this->requestMappings.end();i++)
{
this->requestMappings.erase(i->first);
}
}
}
void WebProjector::onRequest(string url,void (*ptrOnRequest)(Request &,Response &))
{
if(url.length()==0 || ptrOnRequest==NULL) return;
this->requestMappings.insert(pair<string,void (*)(Request &,Response &)>((string)url,ptrOnRequest));
}
void WebProjector::start()
{
FILE *f;
int length;
int i,rc;
int bytesExtracted;
char responseBuffer[1024];  
char requestBuffer[8192]; 
WORD ver;
WSADATA wsaData;
int serverSocketDescriptor;
int clientSocketDescriptor;
struct sockaddr_in serverSocketInformation;
struct sockaddr_in clientSocketInformation;
int len;
int successCode;
ver=MAKEWORD(1,1);
WSAStartup(ver,&wsaData);
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("Unable to create socket\n");
return;
}
serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
char message[101];
if(successCode<0)
{
sprintf(message,"Unable to bind socket to port %d",this->portNumber);
printf("%s\n",message);
WSACleanup();
return;
}
listen(serverSocketDescriptor,10);
len=sizeof(clientSocketInformation);
while(1) 
{
sprintf(message,"Server is ready to accept request on port %d",this->portNumber);
printf("%s\n",message);
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
printf("Unable to accept client connection\n");
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}
bytesExtracted=recv(clientSocketDescriptor,requestBuffer,8192,0);
if(bytesExtracted>0)
{
requestBuffer[bytesExtracted]='\0';
Request *request=parseRequest(requestBuffer);
while(1) 
{
if(request->isClientSideTechnologyResource=='Y')
{
if(request->resource==NULL)
{
f=fopen("index.html","rb");
if(f!=NULL) printf("Sending index.html\n");
if(f==NULL)
{
f=fopen("index.htm","rb");
if(f!=NULL) printf("Sending index.htm\n");
}
if(f==NULL)
{
strcpy(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:163\nConnection:close\n\n<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM Web Projector</title></head><body><h2 style='color:red'>Resource / Not Found</h2></body></html>");
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 page\n");
break; 
}
else
{
fseek(f,0,2);
length=ftell(f);
fseek(f,0,0);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}
fclose(f);
closesocket(clientSocketDescriptor);
break; 
}
}
else
{
length=strlen(request->resource);
int lastIndexOfDot=length-1;
while(lastIndexOfDot>0 && request->resource[lastIndexOfDot]!='.') lastIndexOfDot--;
if(extensionEquals(request->resource+lastIndexOfDot+1,"tpl"))
{
char *rb;
char *tmp;
char r[21];
char a[1025];
int ln,d,j,x,y,k;
string s;
string g="$$${";
f=fopen(request->resource,"rb");
fseek(f,0,2);
length=ftell(f);
fseek(f,0,0);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\n\n");
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
j=0;
d=0;
k=0;
r[0]='\0';
rb=(char *)malloc(sizeof(char)*2049);
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(rb,rc,1,f);
rb[rc]='\0';
ln=strlen(rb);
for(x=0;x<ln;x++) 
{
if(d==1 || d==2)
{
ln=(strlen(rb)+g.length()+strlen(r))+1;
tmp=(char *)malloc(sizeof(char)*(ln+1));
strncpy(a,g.c_str(),j);
a[j]='\0';
sprintf(tmp,"%s%s%s",a,r,rb);
strcpy(rb,tmp);
free(tmp);
j=0;
k=0;
}
if(rb[x]==g[j])
{
y=x;
while(rb[x]==g[j] && rb[x]!='\0' && g[j]!='\0')
{
x++;
j++;
}
if(rb[x]=='\0')
{
d=1;
rb[y]='\0';
break;
}
else if(g[j]=='\0')
{
while(rb[x]!='}' && rb[x]!='\0')
{
r[k]=rb[x];
x++;
k++;
}
r[k]='\0';
if(rb[x]=='\0')
{
d=2;
rb[y]='\0';
break;
}
x++;
s=request->getValue(r);
r[0]='\0';
if(s!="")
{
ln=(strlen(rb)+s.length()-(x-y+1))+1;
tmp=(char *)malloc(sizeof(char)*(ln+1));
strncpy(a,rb,y);
a[y]='\0';
sprintf(tmp,"%s%s%s",a,s.c_str(),rb+x);
strcpy(rb,tmp);
free(tmp);
}
}
j=0;
k=0;
d=0;
x=y+1;
}
}
send(clientSocketDescriptor,rb,strlen(rb),0);
i+=rc;
}
free(rb);
fclose(f);
closesocket(clientSocketDescriptor);
break; 
}
else
{
f=fopen(request->resource,"rb");
if(f!=NULL) printf("Sending %s\n",request->resource);
if(f==NULL)
{
printf("Sending 404 page\n");
char tmp[501];
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM Web Projector</title></head><body><h2 style='color:red'>Resource /%s Not Found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
closesocket(clientSocketDescriptor);
break; 
}
else
{
fseek(f,0,2);
length=ftell(f);
fseek(f,0,0);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nConnection:close\n\n",request->mimeType,length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}
fclose(f);
closesocket(clientSocketDescriptor);
break; 
}
}
}
}
else
{
map<string,void (*)(Request &,Response &)>::iterator iter;
iter=this->requestMappings.find(string("/")+string(request->resource));
if(iter==this->requestMappings.end())
{
printf("Sending 404 page\n");
char tmp[501];
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM Web Projector</title></head><body><h2 style='color:red'>Resource /%s Not Found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
break; 
}
else
{
Response response(clientSocketDescriptor);
iter->second(*request,response);
if(request->forwardTo.length()>0)
{
free(request->resource);
request->resource=(char *)malloc((sizeof(char)*request->forwardTo.length())+1);
strcpy(request->resource,request->forwardTo.c_str());
request->isClientSideTechnologyResource=isClientSideResource(request->resource);
request->mimeType=getMIMEType(request->resource);
request->forwardTo="";
continue;
}
if(request->data!=NULL)
{
for(int k=0;k<request->dataCount;k++) free(request->data[k]);
free(request->data);
}
break; 
}
}
} 
} 
} 
closesocket(clientSocketDescriptor);
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}