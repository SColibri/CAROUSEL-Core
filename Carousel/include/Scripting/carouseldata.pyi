






class Configurations:
	"""
	 Singleton class
		 Contains all modifiable configurations for carousel
	"""

	@staticmethod
	def instance() ->  Configurations : ...
	"""
	 Gets the configurations object
	"""

	SelectedDatabaseString : str = ...
	"""
	 Set the database directive
	"""



class Project:
	"""
	 Project model is the root node for all data generated in any calculation set
	"""

	Id : str = ...
	"""
	 Set Id
	"""

	Name : str = ...
	"""
	 Set name
	"""

	ApiName : str = ...
	"""
	 Set api name that is being used
	"""

	SoftwareName : str = ...
	"""
	 Set name of software used for all calculations
	"""



class CarouselSandbox:
	"""
	 Carousel sandbox is just a class used for playing around with Carousel
		 functionality
	"""

