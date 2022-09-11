# BitcoinAnalyzer with Qt Creator and C++

A program coded with QT Creator and C++ in my spare time. The program fetches bitcoin price data from Goincecko API https://www.coingecko.com/en/api/documentation
and calculates 3 queries based on the fetched data. The GET request functionality is made by SSL-protocol to read, parse, clean and transform the raw data into usable
format.

<h2>NOTICE that you need network connection in order to run the queries successfully!</h2>

<b>The three queries that the user can run (as of now) are the following: </b>

<b>1)</b> The longest bearish (downward) trend in days within the given date range.

<b>2)</b> Date (in format dd.mm.yyyy) with the highest trading volume within the given date range.

<b>3)</b> Best days to sell, buy or hold bitcoin to maximize profits within the given date range. 

## Doumentation

- PDF-documentation file in repository root (bitcoinAnalyzerDocumentation.pdf) OR
- Doxygen documentation (HTML) in ./DoxygenDocumentation/html/index.html

## Executable file now available (tested in Windows10 environment)
bitcoinAnalyzer.exe file now available for download in the releases-section of this repository.

## DISCLAIMERS
* The software is provided as is.
* The software is not commercialized in any way.
