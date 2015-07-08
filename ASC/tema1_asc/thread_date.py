"""
	Acest modul reprezinta thread-ul pentru "citire/scriere in datastore" 
	folosit de un nod din cluster.

	Computer Systems Architecture Course
	Assignment 1 - Cluster Activity Simulation
	March 2014

	Student: NICULA Bogdan Daniel
	Grupa: 334CB
"""

from threading import Thread, Semaphore
from random import randint


class ReaderThread(Thread):
	"""
		Clasa care reprezinta un tip de Thread ce permite accesul(scriere sau citire)
		la datastore-ul unui nod.
	"""
	def __init__(self,node, queue):
		"""
			Constructor.

			@type node: Node
			@param node: Nodul care a creat acest thread.
			@type queue: Queue
			@param queue: Coada de cereri, setata la nivelul Node-ului, din care
					ReaderThread-ul isi poate extrage task-uri.
		"""
		Thread.__init__(self)
		self.node = node
		self.queue = queue
		self.node.datastore.register_thread(self.node, self)

	def run(self):
		"""
			Functia care descrie comportamentul unui ReaderThread. Acesta extrage
			task-uri din coada primita de la nod, si isi incheie activitatea
			la primirea unei comenzi speciale.
		"""
		# Prelucrez elementele din coada pana la primirea unui mesaj de oprire
		while True:
			# Element prelucrat
			value = self.queue.get(True)

			# Caz operatie de citire linie
			if value['operation'] == 'getL':
				# Salvam linia(inclusiv elementul din b) intr-o lista vizibila thread-ului care
				# a cerut aceasta operatie.
				for i in xrange(value["fromIndex"], self.node.matrix_size):
					value["data"][i] = self.node.datastore.get_A(self.node, i)
				
				value["data"][self.node.matrix_size] = self.node.datastore.get_b(self.node)

				# Confirmam ca lista a fost completata
				value["sem"].release()

			# Caz scriere linie in datastore
			elif value["operation"] == "putL":
				# Salvam linia primita(inclusiv elementul b) in datastore
				for i in xrange(value["fromIndex"], self.node.matrix_size):
					self.node.datastore.put_A(self.node, i, value["data"][i])
				
				self.node.datastore.put_b(self.node, value["data"][self.node.matrix_size])

				# Confirmam ca datastore-ul a fost actualizat
				value["sem"].release()
			
			# Caz comanda incheiere task-uri
			elif value["operation"] == "exit":
				self.queue.task_done()
				break
			
			# Caz pentru preluarea unui singur element
			elif value["operation"] == "getElem":
				value["data"][0] = self.node.datastore.get_A(self.node, value["position"])
				value["sem"].release()

			# Caz pentru preluarea elementului din b
			elif value["operation"] == "getB":
				value["data"][0] = self.node.datastore.get_b(self.node)
				value["sem"].release()
			
			self.queue.task_done()



		

