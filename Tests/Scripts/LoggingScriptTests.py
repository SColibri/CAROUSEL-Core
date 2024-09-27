# ----------------------
# Logging script using carousel interface
# -----------------------------------------------------------------------------
# Developer: Sebastian Carrion
# Year: 2024
# 
# Brief: Uses the Carousel logging system
# -----------------------------------------------------------------------------
from carouseldata import *

logger = CarouselLogger.instance()

logger.warning('Warning log from python sccript')
logger.debug('debug log from python sccript')
logger.Info('Info log from python sccript')
logger.error('error log from python sccript')

print('debug log tested :)')
