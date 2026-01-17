#! /usr/bin/python3

import pandas as pd
import matplotlib.pyplot as pl


df = pd.read_csv('results.csv')
df.plot()
pl.show()

