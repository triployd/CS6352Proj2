

#include <iostream>
#include "rv.h"
#include "event.h"



int main()
{
  
  using namespace std;
    
  EventList Elist;
  
  enum {ARRL, ARRH,                          //new arrivals 
  DEP1H, DEP1L, DEP2H, DEP2L,                //departure types
  ARR11L, ARR12L, ARR21L, ARR21H, ARR12H};   //arrivals between the queues             
  
  
  double ph = 0.25;                //probability of high priority customers
  //double pl = (1.0 - ph);         // low priority
  double lambda = 9.0;            // Arrival rate of both priorities
  double mu1 = 25.0;                // Service rate of queue 1 server
  double mu2 = 50.0;

  double clock = 0.0;             // System clock
  int N = 0;                      // Number of customers in system
  int NH1 = 0;
  int NL1 = 0;
  int NH2 = 0;
  int NL2 = 0;
  int N1 = 0;           //# of customers in queue 1;
  int N2 = 0;
  
  double r12l = 0.8;
  double r11l = 0.2;
  double r2d = 0.5;
  double r21 = 0.5;
  
  int Ndep = 0;                   // Number of departures from system
  int NdepH1 = 0;
  int NdepL1 = 0;
  int NdepH2 = 0;
  int NdepL2 = 0;
  double EN = 0.0;                // For calculating E[N]
  //double EN1, EN2;                  // expected value for N1 and N2
  double ENH1,ENH2,ENL1,ENL2;

  int done = 0;                   // End condition satisfied?
  
  
  //for(double lambda = 1.0; lambda <=10.0; lambda += 1.0){
      
  Event* CurrentEvent;

  Elist.insert(exp_rv(lambda),ARRL); // assume 1st customer is a low priority arriving to queue 1
  //Elist.insert(exp_rv(lambda),ARRH); // assume 1st customer is a high priority arriving to queue 1
  
      
  while (!done)
  {            
    CurrentEvent = Elist.get();  
    if (CurrentEvent == 0){
        cout<<"empty list"<<endl;
        break;
    }
    
    //cout<<"breakpoint 1"<<endl;
    double prev = clock;                      // Store old clock value
    clock=CurrentEvent->time;                 // Update system clock      
       
    ENH1 += NH1 * (clock - prev);
    ENH2 += NH2 * (clock - prev);
    ENL1 += NL1 * (clock - prev);
    ENL2 += NL2 * (clock - prev);
    
    switch (CurrentEvent->type) {
        
    case ARRL:   //new arrival to queue 1
        
        NL1++;
        N1++;
        N++;
        //generate next arrival to keep up with the Poisson process
        if (uni_rv()<=ph){
            Elist.insert(clock+exp_rv(lambda),ARRH);
        }else{
            Elist.insert(clock+exp_rv(lambda),ARRL);  //generate next arrival event
        }
        //Elist.insert(clock+exp_rv(lambda),ARR); //  generate next arrival
        if (N1==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu1),DEP1L);   //  generate its departure event       
        }
        break;
      
    case ARRH:                                 // If arrival 
        
        NH2++;
        N2++;
        N++;
        //generate next arrival event no matter what
        if (uni_rv()<=ph){
            Elist.insert(clock+exp_rv(lambda),ARRH);
        }else{
            Elist.insert(clock+exp_rv(lambda),ARRL);  //generate next arrival event
        } 
        if (N2==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu2),DEP2H);   //  generate its departure event
        }
        break;
      
    case DEP1L:                                 // If departure
        
        NL1--;
        N1--;
        NdepL1++;
        
        if(NH1>0){  //if there exist high priority in this queue then generate its departure
                Elist.insert(clock+exp_rv(mu1),DEP1H);
            }else if (NL1>0){
                Elist.insert(clock+exp_rv(mu1),DEP1L);
            }
        if(uni_rv() < r11l){
            //low level customer go to queue 1 again
            Elist.insert(clock, ARR11L); // is this wrong?             
        }else{
            //the low customer goes to queue 2 with probability r12l
            //the time is as same as above as it should enter queue 1
            Elist.insert(clock, ARR12L);
            
        }
        break;
      
    case DEP1H:                                 // If departure
        
        NH1--;
        N1--;
        NdepH1++;
        
        if(NH1>0){  //if there exist high priority in this queue then generate its departure
                Elist.insert(clock+exp_rv(mu1),DEP1H);
            }else if (NL1>0){
                Elist.insert(clock+exp_rv(mu1),DEP1L);
            }
        //it always enter queue 2 if a high customer depart from queue 1:
        Elist.insert(clock, ARR12H);
        
        break;
       
    case DEP2L:                                 // If departure
        
        N2--;
        NL2--;
        NdepL2++;
        
        if(NH2>0){  //if there exist high priority in this queue then generate its departure
                Elist.insert(clock+exp_rv(mu2),DEP2H);
            }else if (NL2>0){
                Elist.insert(clock+exp_rv(mu2),DEP2L);
            }   
        if(uni_rv() < r2d){
            //depart from the system
            Ndep++;
            N--;
            //don't generate arrival to queue 1
        }else{
            //re-enter queue 1
            Elist.insert(clock, ARR21L);
            
        }
        break; 
      
    case DEP2H:                                 // If departure
        
        NH2--;
        N2--;
        NdepH2++;
        
        if(NH2>0){  //if there exist high priority in this queue then generate its departure
                Elist.insert(clock+exp_rv(mu2),DEP2H);
            }else if (NL2>0){
                Elist.insert(clock+exp_rv(mu2),DEP2L);
            }    
        if(uni_rv() < r2d){
            //depart from the system
            Ndep++;
            N--;
        }else{
            //re-enter queue 1
            Elist.insert(clock, ARR21H);
            
        }  
        break;
    
    case ARR11L:
        
        N1++;
        NL1++;
        if (N1==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu1),DEP1L);   //  generate its departure event
        }
        break;
        
    case ARR12L:
        
        N2++;
        NL2++;
        if (N2==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu2),DEP2L);   //  generate its departure event
        }  
        break;
        
    case ARR21L:
        
        N1++;
        NL1++;
        if (N1==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu1),DEP1L);   //  generate its departure event
        } 
        break;
        
    case ARR21H:
        
        N1++;
        NH1++;
        if (N1==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu1),DEP1H);   //  generate its departure event
        } 
        break;
        
    case ARR12H:
        
        N2++;
        NH2++;
        if (N2==1) {                             // If this is the only customer
          Elist.insert(clock+exp_rv(mu2),DEP2H);   //  generate its departure event
        } 
        break;
                
    }
    
    delete CurrentEvent;
    
    if (Ndep > 500000){ done=1; }       // End condition
    
  }
  
  double theta2H = (lambda * ph)/(1 - r21);
  double theta1H = theta2H * r21;
  double theta1L = (lambda * (1 - ph))/(1 - r12l*r21 - r11l);
  double theta2L = theta1L * r12l;
  
  cout << "lambda:  " << lambda <<endl;
  cout << "1:  " <<endl;
  cout << "simulation:  " <<endl;
  cout << "Y1L:  " << NdepL1/clock <<endl;
  cout << "Y1H:  " << NdepH1/clock <<endl;
  cout << "Y2L:  " << NdepL2/clock <<endl;
  cout << "Y2H:  " << NdepH2/clock <<endl;
  cout << "theoretical:  " <<endl;
  cout << "theta1L:  " << theta1L <<endl;
  cout << "theta1H:  " << theta1H <<endl;
  cout << "theta2L:  " << theta2L <<endl;
  cout << "theta2H:  " << theta2H <<endl;
  cout << "2:  " <<endl;
  cout << "simulation:  " <<endl;
  cout << "ENL1:  " << ENL1/clock <<endl;
  cout << "ENH1:  " << ENH1/clock <<endl;
  cout << "ENL2:  " << ENL2/clock <<endl;
  cout << "ENH2:  " << ENH2/clock <<endl;
  cout << "3:  " <<endl;
  cout << "simulation:  " <<endl;
  cout << "ETL1:  " << ENL1/NdepL1 <<endl;
  cout << "ETH1:  " << ENH1/NdepH1 <<endl;
   
  
  
}
