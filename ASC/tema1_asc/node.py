"""
	Acest modul reprezinta nodul computational al cluster-ului

	Computer Systems Architecture Course
	Assignment 1 - Cluster Activity Simulation
	March 2014

	Student: NICULA Bogdan Daniel
	Grupa: 334CB
"""
from threading import *
from thread_date import *
from thread_calcul import *
from barrier import ReusableBarrierCond
from Queue import Queue

class Node:
	"""
		Class that represents a cluster node with computation and storage
		functionalities.
	"""

	def __init__(self, node_id, matrix_size):
		"""
			Constructor.

			@type node_id: Integer
			@param node_id: an integer less than 'matrix_size' uniquely
				identifying the node
			@type matrix_size: Integer
			@param matrix_size: the size of the matrix A
		"""
		self.node_id = node_id
		self.matrix_size = matrix_size
		
		self.datastore = None
		self.nodes = None
		self.queue = Queue()
		
		self.readers = []
		self.semaphoreList = []
		self.semaphoreCount = 0
		self.lock = Lock()
		
		# Initializare bariera pentru sincronizarea nodurilor
		if(node_id == 0):
			self.bar = ReusableBarrierCond(self.matrix_size)

		# Initializare lista semafoare pentru sincronizare intre AlgorithmThread si ReaderThread
		for i in xrange(self.matrix_size):
			self.semaphoreList.append(Semaphore(0))


	def __str__(self):
		"""
			Pretty prints this node.

			@rtype: String
			@return: a string containing this node's id
		"""
		return "Node %d" % self.node_id


	def set_datastore(self, datastore):
		"""
			Gives the node a reference to its datastore. Guaranteed to be called
			before the first call to 'get_x'.

			@type datastore: Datastore
			@param datastore: the datastore associated with this node
		"""
		self.datastore = datastore
		
		# Initializare vector de ReaderThreads
		if datastore.get_max_pending_requests() <= 0:
			for i in xrange(self.matrix_size):
				self.readers.append(ReaderThread(self, self.queue))
		else:
			for i in xrange(self.datastore.get_max_pending_requests()):
				self.readers.append(ReaderThread(self, self.queue))


	def set_nodes(self, nodes):
		"""
			Informs the current node of the other nodes in the cluster. 
			Guaranteed to be called before the first call to 'get_x'.

			@type nodes: List of Node
			@param nodes: a list containing all the nodes in the cluster
		"""
		self.nodes = nodes

		# Setare bariera pentru sincronizarea nodurilor
		if self.node_id != 0:
			for e in nodes:
				# Caut nodul cu id 0 si ii preiau bariera
				if e.node_id == 0:
					self.bar = e.getBarrier()
					break


	def get_x(self):
		"""
			Computes the x value corresponding to this node. This method is
			invoked by the tester. This method must block until the result is
			available.

			@rtype: (Float, Integer)
			@return: the x value and the index of this variable in the solution
				vector
		"""
		
		# Pornesc ReaderThreads si AlgorithmThread
		for element in self.readers:
			element.start()

		computeThread = AlgorithmThread(self, self.bar)


		# Rulam AlgorithmThread
		computeThread.start()
		computeThread.join()

		# Preluam rezultatul salvat in vectorul b
		result = self.get_B() / self.get_elem(self.node_id)

		# Intoarcem rezultat
		return result, self.node_id


	def shutdown(self):
		"""
			Instructs the node to shutdown (terminate all threads). This method
			is invoked by the tester. This method must block until all the
			threads started by this node terminate.
		"""

		# Inchidem ReaderThreads si coada de cereri
		for element in self.readers:
			self.queue.put({"operation":"exit"})
		for element in self.readers:
			element.join()
		self.queue.join()


	def getBarrier(self):
		'''
			Metoda care intoarce bariera folosita de nodul curent.

			@rtype: ReusableBarrierCond
			@return: bariera nodului curent.
		'''
		return self.bar

	def getSemaphore(self):
		'''
			Metoda prin care se intoarce un semafor ce se poate folosi pentru 
			sincronizarea dintre AlgorithmThread si un ReaderThread.
			Avem o lista de maximum 'matrix_size' semafoare pe care le refolosim.
			(mai multe detalii in readme)
		'''
		#  ne asiguram ca un singur thread poate folosi urmatorul semafor disponibil
		self.lock.acquire()
		sem = self.semaphoreList[self.semaphoreCount]
		self.semaphoreCount = (self.semaphoreCount + 1) % len(self.semaphoreList)
		self.lock.release()

		return sem

	def get_elem(self, index):
		'''
			Metoda care pune in coada un request pentru citirea din datastore a unui element.
			Ea asteapta solutionarea requestului si intoarce rezultatul acestuia(elementul dorit).

			@type index: Integer
			@param index: pozitie element dorit din linia aferenta nodului
			@rtype: Float
			@return: valoarea elementului dorit
		'''
		elem = [0]
		sem = self.getSemaphore()

		request = {"operation" : "getElem", "data" : elem, "position" : index, "sem" : sem}
		self.queue.put(request)

		sem.acquire()
		return elem[0]


	def get_B(self):
		'''
			Metoda care pune in coada un request pentru citirea din datastore a 
			elementului din vectorul b.
			Ea asteapta solutionarea requestului si intoarce rezultatul acestuia(elementul dorit).

			@rtype: Float
			@return: valoarea elementului b
		'''
		elem = [0]
		sem = self.getSemaphore()

		request = {"operation" : "getB", "data" : elem, "sem" : sem}
		self.queue.put(request)

		sem.acquire()
		return elem[0]


	def get_line(self, fromIndex = 0):
		'''
			Metoda care pune in coada un request pentru citirea din datastore a unei linii.
			Ea asteapta solutionarea requestului si intoarce rezultatul acestuia
			(linia sub forma unei liste).
			
			NOTA: Vor fi scrise doar elementele din intervalul (fromIndex, matrix_size)
			si elementul din vectorul b.(restul elementelor vor fi 0)

			@type fromIndex: Integer
			@param fromIndex: pozitie in lista de la care se vor prelua valori
			@rtype: List of Float
			@return: o lista de dimensiune matrix_size + 1 reprezentand linia dorita
			la care se adauga elementul din vectorul b
		'''
		dataList = [0 for i in range(self.matrix_size + 1)]
		sem = self.getSemaphore()

		request = {"operation" : "getL", "data" : dataList, "fromIndex" : fromIndex, "sem" : sem}
		self.queue.put(request)

		sem.acquire()
		return dataList


	def put_line(self, line, fromIndex = 0):
		'''
			Metoda care pune in coada un request pentru scrierea in datastore a unei linii.
			Ea asteapta solutionarea requestului.
			Linia care urmeaza a fi scrisa se afla sub forma unei liste, in elementul line. 
			Vor fi scrise doar elementele din intervalul (fromIndex, matrix_size)
			si elementul din vectorul b.

			@type fromIndex: Integer
			@param fromIndex: pozitie in lista de la care se vor prelua valori
			@type line: Lista de Float
			@param line: lista care reprezinta linia care o va suprascrie pe cea existenta in datastore

		'''		
		sem = self.getSemaphore()
		
		request = {"operation" : "putL", "data" : line, "fromIndex" : fromIndex, "sem" : sem}		
		self.queue.put(request)

		sem.acquire()

