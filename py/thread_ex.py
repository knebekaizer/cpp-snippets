#!/usr/bin/env python


import sys
from socket import *
from threading import *
import time

LOGGING = 1

def log( s ):
	if LOGGING:
		print '%s:%s' % ( time.ctime(), s )
		sys.stdout.flush()

class Producer(Thread):
	def __init__( self, cv ):
		Thread.__init__( self )
		self.cv = cv

	def run(self):
	#	log("Producer.run> Sleeping for 15 sec...")
	#	time.sleep(15)
		log("Producer.run> Trying to acquire...")
		if self.cv.acquire(False):
			log("Producer.run> Acquired")
			log("Producer.run> Sleeping for 5 sec...")
			time.sleep(5)
			self.cv.value = 42
			log("Producer.run> Returning value")
			self.cv.notify()
			self.cv.release()
		else:
			log("Producer.run> Failed to acquire")

class Consumer(Thread):
	def __init__( self, cv ):
		Thread.__init__( self )
		self.cv = cv

	def run(self):
		self.cv.acquire()
		log("Consumer> Acquired")
		self.cv.wait(5)
		log("Consumer> Notified or expired, trying to release")
		self.cv.release()
		log("Consumer> Done, value = %s" % self.cv.value)
			
lock = Lock()
cv = Condition(lock)
cv.value = None
#Consumer(cv).start()
#Producer(cv).start()
#time.sleep(1)

count = 42

class ProducerE(Thread):
	def __init__( self, ev ):
		Thread.__init__( self )
		self.ev = ev

	def run(self):
	#	log("Producer.run> Sleeping for 15 sec...")
	#	time.sleep(15)
		log("Producer> Sleeping for 5 sec...")
		assert(not self.ev.isSet())
		time.sleep(5)
		self.ev.value = count
		log("Producer> Returning value")
		self.ev.set()
		time.sleep(1)
		log("Producer> Finished")

class ConsumerE(Thread):
	def __init__( self, ev ):
		Thread.__init__( self )
		self.ev = ev

	def run(self):
		log("Consumer> Waiting...")
		self.ev.wait(2.5)
		log("Consumer> Done, value = %s" % self.ev.value)
		log("Consumer> e.isSet() = %s" % self.ev.isSet())
		self.ev.clear()


e = Event()
e.value = None
for x in range(3):
	count += 1
	c = ConsumerE(e)
	p = ProducerE(e)
	c.start()
	p.start()
	c.join()
	p.join()
	


