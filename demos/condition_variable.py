import shared_memory_py as sm
import time
from multiprocessing import Process


SEGMENT = "sm_py_demo_seg"
CV = "sm_py_demo_cv"
RUNNING = "running"


def ping():

    cv = sm.LockedConditionVariable(SEGMENT,CV,True)
    cv.lock_scope()
    
    print("starting ping ...")
    
    sm.set_bool(SEGMENT,RUNNING,True)
    
    first_iteration = True
    
    while first_iteration or sm.get_bool(SEGMENT,RUNNING):

        first_iteration = False
        cv.wait()
        print("ping!")
        cv.notify_one()

        
def pong():

    cv = sm.LockedConditionVariable(SEGMENT,CV,False)
    cv.lock_scope()
    
    print("starting pong ...")
    
    cv.notify_one()
    
    for iteration in range(1000):

        cv.wait()
        print("pong!")
        cv.notify_one()

    
    sm.set_bool(SEGMENT,RUNNING,False)
        

if __name__== "__main__":

    ping_ = Process(target=ping)
    time.sleep(0.2)
    
    pong_ = Process(target=pong)

    ping_.start()
    pong_.start()

    time.sleep(0.2)

    ping_.join()
    pong_.join()
    
