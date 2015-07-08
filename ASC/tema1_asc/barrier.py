"""
    Acest modul reprezinta bariere reentranta folosita pentru sincronizarea nodurilor din cluster.

    NOTA: Acest modul a fost preluat din scheletul pentru laboratorul 3.

    Computer Systems Architecture Course
    Assignment 1 - Cluster Activity Simulation
    March 2014

    Student: NICULA Bogdan Daniel
    Grupa: 334CB
"""

import sys
import threading
from threading import *


class ReusableBarrierCond():
    """ Bariera reentranta, implementata folosind o variabila conditie """

    def __init__(self, num_threads):
        self.num_threads = num_threads
        self.count_threads = self.num_threads
        self.cond = Condition(Lock())

    def wait(self):
        self.cond.acquire()       # intra in regiunea critica
        self.count_threads -= 1;  
        if self.count_threads == 0:
            self.cond.notify_all() # trezeste toate thread-urile, acestea vor putea reintra  in regiunea critica dupa release
            self.count_threads=self.num_threads  
        else:          
            self.cond.wait();    # iese din regiunea critica, se blocheaza, cand se deblocheaza face acquire pe lock
        self.cond.release();     # iesim din regiunea critica