"""
	Acest modul reprezinta thread-ul "pentru calcule" folosit de un nod din cluster.

	Computer Systems Architecture Course
	Assignment 1 - Cluster Activity Simulation
	March 2014

	Student: NICULA Bogdan Daniel
	Grupa: 334CB
"""


from threading import Thread, Semaphore
from random import randint
from barrier import ReusableBarrierCond



class AlgorithmThread(Thread):
	"""
		Clasa care reprezinta un tip de Thread ce realizeaza toate calculele,
		implementand practic algoritmul de eliminare gaussiana.
		Pentru fiecare nod se creeaza un singur Algorithm thread.
	"""
	def __init__(self, node, barrier):
		"""
			Constructor.

			@type node: Node
			@param node: Nodul care a creat acest thread.
			@type barrier: ReusableBarrierCond
			@param barrier: Bariera reentranta initializata la nivelul nodurilor
					care permite sincronizarea AlgorithmThread-urilor
		"""
		Thread.__init__(self)
		self.node = node
		self.barrier = barrier		
		self.node.datastore.register_thread(self.node, self)

	def run(self):
		"""
			Functia care descrie comportamentul unui AlgorithmThread. 
			Acesta urmeaza pasii algoritmului de eliminare gaussiana descrisi in README
			AlgorithmThread-ul interactioneaza cu alte noduri prin intermediul
			metodelor(get_line, get_elem, getB, put_line) create la nivelul
			clasei Node.
		"""
		# Numar pas curent in cadrul iteratiei
		step = 0

		# Valori retinute in variabile locale pentru a nu fi cerute pe parcurs
		matrix_size = self.node.matrix_size
		node_id = self.node.node_id

		while True:

			if step >= matrix_size:
				break

			# Verific daca linia curenta poate fi folosita ca "pivot" sau daca trebuie
			# sa gasesc o alta linie valida si sa fac swap
			if step == node_id:
				if self.node.get_elem(step) == 0:
					# Caut o linie cu care sa fac swap
					for element in self.node.nodes:
						if element.node_id > step and element.get_elem(step) != 0:
							# Swap-ul propriu-zis
							line = self.node.get_line(step)
							self.node.put_line(element.get_line(),step)
							element.put_line(line,step)
							break

			# Toate thread-urile de calcul din toate nodurile vor astepta nodul "pivot"
			# pentru a se asigura ca matricea este corecta
			self.barrier.wait()

			if step != node_id:
				
				# Caut referinta nodului pivot
				if self.node.nodes[step].node_id == step:
					pivot = self.node.nodes[step]
				else:
					# Cazul in care lista nodes a unui nod nu contine nodurile in ordine
					for i in self.node.nodes:
						if i.node_id == step:
							pivot = i
							break
				
				# Extrag linie nod curent si linie pivot
				nodeLine = self.node.get_line(step)
				pivotLine = pivot.get_line(step)
				
				# Calculez factorul de reducere
				factor = nodeLine[step] / pivotLine[step]

				# Scad din linia nodului curent linia pivotului * factor reducere
				nodeLine[step] = 0
				for i in xrange(step + 1,matrix_size + 1):
					nodeLine[i] = nodeLine[i] - factor * pivotLine[i]

				# Actualizez linia proprie
				self.node.put_line(nodeLine,step)

			step = step + 1
			self.barrier.wait()

