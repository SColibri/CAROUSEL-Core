from typing import List


class Configurations:
	"""
	 Singleton class
		 Contains all modifiable configurations for carousel
	"""

	@staticmethod
	def instance() ->  Configurations : ...
	"""
	 Assignment operator (removed)
			 </summary>
			void operator=(const Configurations&) = delete;

			 <summary>
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

	def load(self, rawData : List[str]): ... 
	"""
	
	"""

	def get_table_structure(self): ...
	"""
	
	"""



class CarouselSandbox:
	"""
	 Carousel sandbox is just a class used for playing around with Carousel
		 functionality
	"""

	def hello_carousel(self): ...
	"""
	 Carousel sandbox is just a class used for playing around with Carousel
		 functionality
		 </summary>
		class CarouselSandbox
		{
		public:
			CarouselSandbox() = default;

			 <summary>
			 Says hello
	"""



class CarouselLogger:
	"""
	 Logger service for carousel
	"""

	@staticmethod
	def instance() ->  CarouselLogger : ...
	"""
	 Assignment operator (removed)
			 </summary>
			void operator=(const CarouselLogger&) = delete;

			 <summary>
			 Gets the logger object
	"""

	def Info(self, message : str): ... 
	"""
	 Log info
	"""

	def debug(self, message : str): ... 
	"""
	 Log debug
	"""

	def error(self, message : str): ... 
	"""
	 Log error
	"""

	def warning(self, message : str): ... 
	"""
	 Log warning
	"""

