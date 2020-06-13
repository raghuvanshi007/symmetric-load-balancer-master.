//Webserver implemented per thread
//Sender-Initiated

#include <cstdlib>
#include<iostream>

#define numberOfPollingTrials 5
using namespace std;

int nAddresses;
float REQUEST_BUFFER_SIZE =10.0;
float loadThreshold=0.0;

//return servername
webserver::webserver(){
    serverName=' ';
}
webserver::webserver(char c){
    serverName=c;
}
char webserver:: getName()
{
    return serverName;
}

void webserver :: setAverageThreshold()
{
    float loadSum = 0.0;
    for (int i = 0; i < webserverPointers.size(); i++)
    {
        loadSum += webserverPointers[i]->calculateServerLoad();
    }
    loadSum += calculateServerLoad();
    loadThreshold = loadSum / (float)(webserverPointers.size() + 1);
}
float webserver ::getAverageThreshold()
{
    float loadSum = 0.0;
    cout << webserverPointers.size()<<" ";
     for (int i = 0; i < webserverPointers.size(); i++)
    {
        loadSum += webserverPointers[i]->calculateServerLoad();
        cout << loadSum << endl;
    }
    loadSum += calculateServerLoad();
    
    loadThreshold = loadSum / ((float)webserverPointers.size() + 1.0);
    return loadSum;
}

//simple method to push method on reciever end when server was overloaded
void webserver::migrateRequestOnLoadSender(request req){
    requestQueue.push(req);
}

//Server Initiated load Balancing Algorithm
void webserver :: ServerInitiatedLoadBalancing(request req){
    int p = numberOfPollingTrials;
    nAddresses = webserverPointers.size();

    while(p--){
        int i = rand() % (nAddresses);
        webserver *w = webserverPointers[i];

        if(w->calculateServerLoad()<loadThreshold){
            
            w->migrateRequestOnLoadSender(req);
            cout << "Server Overloaded : Avg Load : "<< loadThreshold<<endl;
            cout<<"Migrated task to : "<<w->getName() << endl;
            break;
        }
    }
    if(p==0){
        cout<<"All Servers busy. Remove some tasks first"<<endl;
    }
}
void webserver::initRequest(request req){
        requestQueue.push(req);  
}

//Method to add request
void webserver::addRequest(request req){
    setAverageThreshold();
    //cout<<(double)getAverageThreshold()<<" *****"<<endl;
    float serverLoad = calculateServerLoad();
    if(serverLoad>loadThreshold){
        
        ServerInitiatedLoadBalancing(req);
    }
    else
        requestQueue.push(req);  
}

//return request from front of the queue;
request webserver:: getRequest(){
    request r;
    if (requestQueue.empty()){
        cout << "Cannot return task from SERVER : " << serverName << " - Request Buffer empty";
    }
    else{
        //cout task removed from server requestQueue.front()
        r = requestQueue.front();
        requestQueue.pop();
    }
    return r;
}
request webserver::migrateRequestOnLoadReceiver(){
    request r=requestQueue.front();
    requestQueue.pop();
    return r;
}

int webserver :: receiverInitiatedLoadBalancing(){
    int p = numberOfPollingTrials;
    nAddresses = webserverPointers.size();

    while(--p){
        int i = rand() % (nAddresses);
        webserver *w = webserverPointers[i];

        if(w->calculateServerLoad()>loadThreshold){
            request r;
            r = w->migrateRequestOnLoadReceiver();
            addRequest(r);
            cout << "Server Underloaded : Avg Load : "<<loadThreshold<<endl;
            cout<<" Migrated task from : "<<w->getName() << endl;
            return 1;
        }
    }
    cout<<"No Servers Overloaded. Nothing to migrate."<<endl;
    return 0;
}

void webserver::removeRequest()
{   if(requestQueue.empty()){
        cout<<"No requests to Remove"<<endl;
    }
    else{
        requestQueue.pop();
        setAverageThreshold();
        float serverLoad = calculateServerLoad();
        if(serverLoad<loadThreshold)
            receiverInitiatedLoadBalancing();  
    }       
}


//return current server load
float webserver:: calculateServerLoad()
{
    float requestQueueSize = (float)getRequestQueueSize();
    //cout<<requestQueueSize<<endl;
    float serverLoad = requestQueueSize/REQUEST_BUFFER_SIZE;
    return serverLoad;
}
//Handler to populate server addresses
void webserver:: addWebserverAddress(webserver *webserverPointer){
    webserverPointers.push_back(webserverPointer);
    
}

//method to pring server data for debugging
void webserver::displayWebserverAddress(){
    for(int i=0;i<webserverPointers.size();i++){
        webserverPointers[i]->displayData();
        cout<<endl;
    }
}

//to display server name and load
void webserver::displayData(){
    cout<<"Server Name: " <<getName()<<" Load: "<<(float)calculateServerLoad()<<endl;
}

//helper to return buffer size
int webserver::getRequestQueueSize(){
    return requestQueue.size();
}


