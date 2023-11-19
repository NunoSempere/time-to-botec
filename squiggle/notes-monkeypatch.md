I can't currently figure out how to change the number of samples from within squiggle for mixtures, 
so instead I monkey patched it

- https://github.com/quantified-uncertainty/squiggle/issues/2560
- grep -r . -e defaultSampleCount
- sed -i 's/defaultSampleCount: 1000/defaultSampleCount: 1000000/g' node_modules/@quri/squiggle-lang/src/magicNumbers.ts node_modules/@quri/squiggle-lang/dist/magicNumbers.js

