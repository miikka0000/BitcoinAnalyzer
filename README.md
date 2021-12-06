# BitcoinAnalyzer

A program coded with QT Creator and C++ that fetches bitcoin price data from Goincecko API https://www.coingecko.com/en/api/documentation
and calculates 3 queries based on the fetched data. The GET request functionality is made by SSL-protocol to read, parse, clean and transform the raw data into usable
format.

<b>The three queries that the user can run (as of now) are the following: </b>

- A) The longest bearish (downward) trend in days within the given date range.

- B) Date (in format dd.mm.yyyy) with the highest trading volume within the given date range.

- C) Best days to sell, buy or hold bitcoin to maximize profits withing the given date range. 

## Doumentation

- PDF-documentation file in repository root (bitcoinAnalyzerDocumentation.pfd) OR
- Doxygen documentation (HTML) in ./DoxygenDocumentation/html/index.html




