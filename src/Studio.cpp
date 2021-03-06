
#include "../include/Studio.h"
#include "../include/Workout.h"
#include "../include/Action.h"
#include    <iterator>

using namespace std;

Studio::Studio():open(), trainers(), workout_options(), actionsLog() {
}


Studio::Studio(const std::string &configFilePath):open(), trainers(), workout_options(), actionsLog() {
    //here we define the list of trainers and list of workouts plains
    fstream newfile;
    newfile.open(configFilePath,ios::in);
    if (newfile.is_open()){   //checking whether the file is open
        string tp;
        //parse the first line - number of trainers
        getline(newfile, tp); // first row is "#trainers" dont need it
        while(!tp.empty()){
            //#trainers
            int NumOfTrainers;
            if(tp == "# Number of trainers"){
                getline(newfile, tp); // We get now the data of number of trainers
                sscanf(tp.data(), "%d", &NumOfTrainers); // string to int, because we know what is the second line
                getline(newfile, tp); // We finished with this line and go to the next one
            }
            //#capacity

            if(tp == "# Traines") {
                getline(newfile, tp); //We get now the data of trainers capacity
                int n = tp.length();
                char Trainers_Capacitys[n+1];
                strcpy(Trainers_Capacitys, tp.c_str()); //here we change the trainers capacity string into char array
                for(int i = 0; i <n ; i++){
                    int char_to_num = (Trainers_Capacitys[i] - '0');
                    if(char_to_num != -4) {
                        trainers.push_back(new Trainer(char_to_num)); //convert char to int and insert to trainer vectos
                    }
                }
                getline(newfile, tp); // We finished with this line and go to the next one
            }
            //#workout
            if(tp == "# Work Options") {
                int Workout_id = 0;
                while(getline(newfile, tp) && !tp.empty()){
                    vector<string> curr_row;
                    stringstream s_stream(tp);
                    while(s_stream.good()) {
                        string substr;
                        getline(s_stream, substr, ',');
                        curr_row.push_back(substr);
                    }
                    string name;
                    string stype;
                    string sprice;
                    for (int i = 0; i < static_cast<int>(curr_row.size()); i++) {
                        switch(i){
                            case 0:
                                name = curr_row[i];
                                break;
                            case 1:
                                stype = curr_row[i];
                                break;
                            case 2:
                                sprice = curr_row[i];
                                break;
                        }
                    }
                    curr_row.empty();
                    WorkoutType type;

                    if(stype == " Anaerobic"){
                        type = WorkoutType::ANAEROBIC;
                    }
                    else if(stype == " Mixed"){
                        type = WorkoutType::MIXED;
                    }
                    else if(stype == " Cardio"){
                        type = WorkoutType::CARDIO;
                    }
                    int price = stoi(sprice);
                    Workout w = Workout(Workout_id,name,price,type);
                    workout_options.push_back(w);
                    Workout_id++;
                }
            }
            //blank line skip
            getline(newfile, tp);

            //exit file when done, AKA we finished with workout types
            if(static_cast<int>(workout_options.size()) > 0) {
                newfile.close(); //close the file object.
                break;
            }

        }

    }



}

bool Studio::get_status() {
    return open;
}

void Studio::setStatus() {
    open = true;
}

void Studio::close_studio() {
    open = false;
}

void Studio::start() {
    open = true;
    std::cout <<"Studio is now open!" << std::endl;
}

int Studio::getNumOfTrainers() const {
    return  static_cast<int>(trainers.size());
}

Trainer *Studio::getTrainer(int tid) {
    return trainers[tid];
}

vector<Trainer *> &Studio::getTrainers() {
    return trainers;
}

std::vector<Workout> &Studio::getWorkoutOptions() {
    return workout_options;

}

void Studio::add_action_to_log(BaseAction *toadd) {
    actionsLog.push_back(toadd);
}


const std::vector<BaseAction *> &Studio::getActionsLog() const {
    return actionsLog;
}

//destructor
Studio::~Studio() {
    for (int i = 0; i < static_cast<int>(trainers.size()); ++i) {
        delete trainers[i];
        trainers[i] = nullptr;
    }
    for (int i = 0; i <static_cast<int>(actionsLog.size()) ; ++i) {
        if (actionsLog[i]) {
            if(actionsLog[i]->toString().substr(0,4) == "open")
                delete dynamic_cast<OpenTrainer*>(actionsLog[i]);
            else
                delete actionsLog[i];

            actionsLog[i] = nullptr;
        }
    }
    trainers.clear();
    actionsLog.clear();
    workout_options.clear();
}

//copy constructor
Studio::Studio(const Studio &other):open(other.open), trainers(other.trainers), workout_options(other.workout_options), actionsLog(other.actionsLog) {
    trainers.clear();
    actionsLog.clear();
    for (Trainer *t: other.trainers) {
        trainers.push_back(new Trainer(*t));
    }
    for (BaseAction *log : other.actionsLog) {
        if (log->toString().substr(0,4) == "open")
            actionsLog.push_back(new OpenTrainer("",dynamic_cast<OpenTrainer*>(log)->getids()));
        if (log->toString().substr(0,4) == "orde")
            actionsLog.push_back(new Order("",dynamic_cast<Order*>(log)->getids()));
        if (log->toString().substr(0,4) == "move") {
            vector<int> tmp = dynamic_cast<MoveCustomer*>(log)->getids();
            if(static_cast<int>(tmp.size()) == 3) {
                int src = tmp[0];
                int dst = tmp[1];
                int id = tmp[2];
                actionsLog.push_back(new MoveCustomer("",src,dst,id));
            }
        }
        if (log->toString().substr(0,4) == "clos")
            actionsLog.push_back(new Close("",dynamic_cast<Close*>(log)->getids()));
        if (log->toString().substr(0,4) == "work")
            actionsLog.push_back(new PrintWorkoutOptions(""));
        if (log->toString().substr(0,4) == "stat")
            actionsLog.push_back(new PrintTrainerStatus(""));
        if (log->toString().substr(0,4) == "log ")
            actionsLog.push_back(new PrintActionsLog(""));
        if (log->toString().substr(0,4) == "back")
            actionsLog.push_back(new BackupStudio(""));
        if (log->toString().substr(0,4) == "rest")
            actionsLog.push_back(new RestoreStudio(""));

    }
}

//move constructor
Studio::Studio(Studio &&other):open(other.open), trainers(other.trainers), workout_options(other.workout_options), actionsLog(other.actionsLog) {
    trainers.clear();
    actionsLog.clear();
    for (Trainer *t: other.trainers) {
        trainers.push_back(new Trainer(*t));
    }
    for (BaseAction *log : other.actionsLog) {
        if (log->toString().substr(0,4) == "open")
            actionsLog.push_back(new OpenTrainer("",dynamic_cast<OpenTrainer*>(log)->getids()));
        if (log->toString().substr(0,4) == "orde")
            actionsLog.push_back(new Order("",dynamic_cast<Order*>(log)->getids()));
        if (log->toString().substr(0,4) == "move") {
            vector<int> tmp = dynamic_cast<MoveCustomer*>(log)->getids();
            if(static_cast<int>(tmp.size()) == 3) {
                int src = tmp[0];
                int dst = tmp[1];
                int id = tmp[2];
                actionsLog.push_back(new MoveCustomer("",src,dst,id));
            }
        }
        if (log->toString().substr(0,4) == "clos")
            actionsLog.push_back(new Close("",dynamic_cast<Close*>(log)->getids()));
        if (log->toString().substr(0,4) == "work")
            actionsLog.push_back(new PrintWorkoutOptions(""));
        if (log->toString().substr(0,4) == "stat")
            actionsLog.push_back(new PrintTrainerStatus(""));
        if (log->toString().substr(0,4) == "log ")
            actionsLog.push_back(new PrintActionsLog(""));
        if (log->toString().substr(0,4) == "back")
            actionsLog.push_back(new BackupStudio(""));
        if (log->toString().substr(0,4) == "rest")
            actionsLog.push_back(new RestoreStudio(""));

    }
}

//copy assignment
Studio &Studio::operator=(const Studio &other) {
    if (this != &other) {
        clear();
        open = other.open;
        for (Workout w: other.workout_options) {
            int id = w.getId();
            int price = w.getPrice();
            WorkoutType a = w.getType();
            string name = w.getName();
            workout_options.push_back(Workout(id,name,price,a));
        }
        for (Trainer *t: other.trainers) {
            trainers.push_back(new Trainer(*t));
        }
        for (BaseAction *log : other.actionsLog) {
            if (log->toString().substr(0,4) == "open")
                actionsLog.push_back(new OpenTrainer("",dynamic_cast<OpenTrainer*>(log)->getids()));
            if (log->toString().substr(0,4) == "orde")
                actionsLog.push_back(new Order("",dynamic_cast<Order*>(log)->getids()));
            if (log->toString().substr(0,4) == "move") {
                vector<int> tmp = dynamic_cast<MoveCustomer*>(log)->getids();
                if(static_cast<int>(tmp.size()) == 3) {
                    int src = tmp[0];
                    int dst = tmp[1];
                    int id = tmp[2];
                    actionsLog.push_back(new MoveCustomer("",src,dst,id));
                }
            }
            if (log->toString().substr(0,4) == "clos")
                actionsLog.push_back(new Close("",dynamic_cast<Close*>(log)->getids()));
            if (log->toString().substr(0,4) == "work")
                actionsLog.push_back(new PrintWorkoutOptions(""));
            if (log->toString().substr(0,4) == "stat")
                actionsLog.push_back(new PrintTrainerStatus(""));
            if (log->toString().substr(0,4) == "log ")
                actionsLog.push_back(new PrintActionsLog(""));
            if (log->toString().substr(0,4) == "back")
                actionsLog.push_back(new BackupStudio(""));
            if (log->toString().substr(0,4) == "rest")
                actionsLog.push_back(new RestoreStudio(""));

        }

    }
    return *this;
}

//move assignment
Studio &Studio::operator=(Studio &&other) {
    if(this != &other) {
        clear();
        open = other.open;
        for (Workout w: other.workout_options) {
            int id = w.getId();
            int price = w.getPrice();
            WorkoutType a = w.getType();
            string name = w.getName();
            workout_options.push_back(Workout(id,name,price,a));
        }
        for (Trainer *t: other.trainers) {
            trainers.push_back(new Trainer(*t));
        }
        for (BaseAction *log : other.actionsLog) {
            if (log->toString().substr(0,4) == "open")
                actionsLog.push_back(new OpenTrainer("",dynamic_cast<OpenTrainer*>(log)->getids()));
            if (log->toString().substr(0,4) == "orde")
                actionsLog.push_back(new Order("",dynamic_cast<Order*>(log)->getids()));
            if (log->toString().substr(0,4) == "move") {
                vector<int> tmp = dynamic_cast<MoveCustomer*>(log)->getids();
                if(static_cast<int>(tmp.size()) == 3) {
                    int src = tmp[0];
                    int dst = tmp[1];
                    int id = tmp[2];
                    actionsLog.push_back(new MoveCustomer("",src,dst,id));
                }
            }
            if (log->toString().substr(0,4) == "clos")
                actionsLog.push_back(new Close("",dynamic_cast<Close*>(log)->getids()));
            if (log->toString().substr(0,4) == "work")
                actionsLog.push_back(new PrintWorkoutOptions(""));
            if (log->toString().substr(0,4) == "stat")
                actionsLog.push_back(new PrintTrainerStatus(""));
            if (log->toString().substr(0,4) == "log ")
                actionsLog.push_back(new PrintActionsLog(""));
            if (log->toString().substr(0,4) == "back")
                actionsLog.push_back(new BackupStudio(""));
            if (log->toString().substr(0,4) == "rest")
                actionsLog.push_back(new RestoreStudio(""));

        }
        other.trainers.clear();
        other.actionsLog.clear();
    }
    return *this;
}

void Studio::clear() {
    for (int i = 0; i < static_cast<int>(trainers.size()); ++i) {
        delete trainers[i];
        trainers[i] = nullptr;
    }
    for (int i = 0; i <static_cast<int>(actionsLog.size()) ; ++i) {
        if (actionsLog[i]) {
            if(actionsLog[i]->toString().substr(0,4) == "open")
                delete dynamic_cast<OpenTrainer*>(actionsLog[i]);
            else
                delete actionsLog[i];

            actionsLog[i] = nullptr;
        }
    }
    workout_options.clear();
    trainers.clear();
    actionsLog.clear();
}


