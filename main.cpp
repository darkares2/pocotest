#include <iostream>
#include <sstream>
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/TaskNotification.h"
#include "Poco/Observer.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Parser.h"

using Poco::FileInputStream;
using Poco::StreamCopier;
using Poco::Observer;
using namespace std;

class SampleTask: public Poco::Task
{
public:
    SampleTask(const std::string& name): Task(name)
    {
    }
    void runTask()
    {
        for (int i = 0; i < 100; ++i)
        {
            setProgress(float(i)/100); // report progress
            if (sleep(1))
                break;
        }
    }
};

class JsonTask: public Poco::Task
{
public:
    JsonTask(const std::string& name): Task(name)
    {
    }
    void runTask()
    {
        sleep(1000);
        try {
            FileInputStream input = FileInputStream("test.json");
            ostringstream oss;
            StreamCopier::copyStream(input, oss);
            Poco::JSON::Parser jsonParser;
            auto result = jsonParser.parse(oss.str());
            auto root = result.extract<Poco::JSON::Object::Ptr>();
            Poco::DynamicStruct ds = *root;
            cout << "ID=" << ds["id"].toString() << ", type=" << ds["type"].toString() << ", fun=" << ds["fun"].isEmpty() << endl;
            Poco::DynamicStruct batters = ds["batters"].extract<Poco::DynamicStruct>();
            cout << "Got batters, wonder if batter is an array?: " << batters["batter"].isArray() << endl;
            Poco::Dynamic::Var batter = batters["batter"];
            //Poco::JSON::Array::Ptr batterList = batter.extract<Poco::JSON::Array::Ptr>();
            //cout << "Got batterlist" << endl;

            for (const auto & it : batters["batter"])
            {
                cout << "Item: " << it.isStruct() << endl;
                auto batter = it.extract<Poco::DynamicStruct>();
                cout << "Batter: " << batter["id"].toString() << ", type=" << batter["type"].toString() << endl;
            }
        } catch(Poco::Exception& e) {
            cout << "Failed: " << e.displayText() << endl;
        }
    }
};

class ProgressHandler
{
public:
    void onProgress(Poco::TaskProgressNotification* pNf)
    {
        std::cout << pNf->task()->name() << " progress: " << pNf->progress() << std::endl;
        pNf->release();
    }
    void onFinished(Poco::TaskFinishedNotification* pNf)
    {
        std::cout << pNf->task()->name() << " finished." << std::endl;
        pNf->release();
    }
};


int main() {
    std::cout << "Hello, World!" << std::endl;
    Poco::TaskManager tm;
    ProgressHandler pm;
    tm.addObserver(
            Observer<ProgressHandler, Poco::TaskProgressNotification>
                    (pm, &ProgressHandler::onProgress)
    );
    tm.addObserver(
            Observer<ProgressHandler, Poco::TaskFinishedNotification>
                    (pm, &ProgressHandler::onFinished)
    );
    tm.start(new SampleTask("Task 1")); // tm takes ownership
    tm.start(new SampleTask("Task 2"));
    tm.start(new JsonTask("Task Json"));
    tm.joinAll();
    return 0;
}
