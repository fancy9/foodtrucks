#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WResource>
#include <Wt/WServer>
#include <Wt/Http/Response>

#include <iostream>
#include <string>
#include "mongo/client/dbclient.h" // for the driver
using namespace std;
using namespace Wt;
using namespace mongo;

class DBtest {
    void run() {
        mongo::DBClientConnection c;
        c.connect("localhost");
    }

public:
    DBtest() {
        cout << "before run()" << endl;
    }
    void start() {
        mongo::client::initialize();
        try {
            run();
            std::cout << "connected ok" << std::endl;
        } catch (const mongo::DBException &e) {
            std::cout << "caught " << e.what() << std::endl;
        }
        //return EXIT_SUCCESS;
    }

};

class MyResource: public Wt::WResource {
public:
    MyResource(Wt::WObject *parent = 0) :
            Wt::WResource(parent) {
    }
    ~MyResource() {
        beingDeleted();
    }
    void handleRequest(const Wt::Http::Request& request,
            Wt::Http::Response& response) {
    	response.addHeader("Content-Type", "application/json; charset=utf-8");
        response.addHeader("Access-Control-Allow-Origin", "*");
    	response.setMimeType("application/json");

//    	response.out() << "<html>\n"
//    	    "<body>\n";

        std::string startLat = *(request.getParameter("startLat"));
        std::string startLng = *(request.getParameter("startLng"));
        std::string endLat = *(request.getParameter("endLat"));
        std::string endLng = *(request.getParameter("endLng"));

        try {
            mongo::DBClientConnection c;
            c.connect("localhost");

            Query query = MONGO_QUERY( "Latitude" << mongo::GT << atof(startLat.c_str()) << mongo::LT << atof(endLat.c_str())
                    << "Longitude" << mongo::GT << atof(startLng.c_str()) << mongo::LT << atof(endLng.c_str()) );
            string ns = "mydb.things";

            cout<<query.toString()<<endl;
            std::auto_ptr<mongo::DBClientCursor> cursor = c.query(ns, query);

            string data = "[ ";
            int first = 1;
            while(cursor->more()) {
                BSONObj bson = cursor->next();
                if(!first) {
                	data+= ", \n";
                } else {
                	first = 0;
                }
                data += bson.jsonString();
            }
            data += " ]\n";
            cout<<data.size()<<endl;
            response.out() << data;

        } catch (const mongo::DBException &e) {
            std::cout << "caught " << e.what() << std::endl;
        }

//        response.out() << "</body>\n</html>";
        // make sql query

        // make the response
    }

};


WApplication *createApplication(const WEnvironment& env)
{
return new WApplication(env);
}

int main(int argc, char **argv) {
    //DBtest dbt;
    //dbt.start();
    try {
        Wt::WServer server(argv[0]);

        server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

        MyResource resource;
        server.addResource(&resource, "/test");

        server.addEntryPoint(Application, &createApplication);
        if (server.start()) {
            int sig = Wt::WServer::waitForShutdown(argv[0]);

            std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
            server.stop();
        }
    } catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }
}
