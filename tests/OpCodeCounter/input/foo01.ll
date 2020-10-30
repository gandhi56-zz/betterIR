; ModuleID = 'foo.ll'
source_filename = "foo.ll"

declare void @printInt(i32)

declare void @printVec(i32*, i32)

define i32 @main(i32 %0, i8** %1) {
entry:
  %lenPtr = alloca i32
  store i32 6, i32* %lenPtr
  %idxPtr = alloca i32
  store i32 0, i32* %idxPtr
  %val = alloca i32
  store i32 0, i32* %val
  %zeroCheckElems = load i32, i32* %lenPtr
  %LTEZero = icmp sle i32 %zeroCheckElems, 0
  br i1 %LTEZero, label %setZero, label %createArr

setZero:                                          ; preds = %entry
  store i32 0, i32* %lenPtr
  br label %createArr

createArr:                                        ; preds = %setZero, %entry
  %numElem = load i32, i32* %lenPtr
  %arr = alloca i32, i32 %numElem
  br label %cond

cond:                                             ; preds = %inRange, %createArr
  %idx = load i32, i32* %idxPtr
  %val1 = load i32, i32* %val
  %cmp = icmp slt i32 %idx, %numElem
  br i1 %cmp, label %inRange, label %outRange

inRange:                                          ; preds = %cond
  %vecPtr = getelementptr i32, i32* %arr, i32 %idx
  store i32 %val1, i32* %vecPtr
  %val2 = add i32 %val1, 1
  store i32 %val2, i32* %val
  %idx3 = add i32 %idx, 1
  store i32 %idx3, i32* %idxPtr
  br label %cond

outRange:                                         ; preds = %cond
  %lenPtr4 = alloca i32
  store i32 -4, i32* %lenPtr4
  %idxPtr5 = alloca i32
  store i32 0, i32* %idxPtr5
  %val6 = alloca i32
  store i32 5, i32* %val6
  %zeroCheckElems10 = load i32, i32* %lenPtr4
  %LTEZero11 = icmp sle i32 %zeroCheckElems10, 0
  br i1 %LTEZero11, label %setZero7, label %createArr8

setZero7:                                         ; preds = %outRange
  store i32 0, i32* %lenPtr4
  br label %createArr8

createArr8:                                       ; preds = %setZero7, %outRange
  %numElem12 = load i32, i32* %lenPtr4
  %arr13 = alloca i32, i32 %numElem12
  br label %cond9

cond9:                                            ; preds = %inRange20, %createArr8
  %idx14 = load i32, i32* %idxPtr5
  %val15 = load i32, i32* %val6
  %cmp16 = icmp slt i32 %idx14, %numElem12
  br i1 %cmp16, label %inRange20, label %outRange21

inRange20:                                        ; preds = %cond9
  %vecPtr17 = getelementptr i32, i32* %arr13, i32 %idx14
  store i32 %val15, i32* %vecPtr17
  %val18 = add i32 %val15, 1
  store i32 %val18, i32* %val6
  %idx19 = add i32 %idx14, 1
  store i32 %idx19, i32* %idxPtr5
  br label %cond9

outRange21:                                       ; preds = %cond9
  %newVector = alloca i32, i32 %numElem12
  %idxidxptr = alloca i32
  store i32 0, i32* %idxidxptr
  br label %indexCond

indexCond:                                        ; preds = %indexBody, %outRange21
  %currIdx = load i32, i32* %idxidxptr
  %valueAtIdxPtr = getelementptr i32, i32* %arr13, i32 %currIdx
  %valueAtIdx = load i32, i32* %valueAtIdxPtr
  %valPtr = alloca i32
  %cmpRes = icmp slt i32 %currIdx, %numElem12
  br i1 %cmpRes, label %setIndexCond, label %indexOut

setIndexCond:                                     ; preds = %indexCond
  %cmpIdxValToVecNumElem = icmp slt i32 %valueAtIdx, %numElem
  %cmpIdxValGEZero = icmp sge i32 %valueAtIdx, 0
  %"&cmp" = and i1 %cmpIdxValToVecNumElem, %cmpIdxValGEZero
  br i1 %"&cmp", label %setIndexVal, label %setIndexZero

setIndexVal:                                      ; preds = %setIndexCond
  %valPtr22 = getelementptr i32, i32* %arr, i32 %valueAtIdx
  %2 = load i32, i32* %valPtr22
  store i32 %2, i32* %valPtr
  br label %indexBody

setIndexZero:                                     ; preds = %setIndexCond
  store i32 0, i32* %valPtr
  br label %indexBody

indexBody:                                        ; preds = %setIndexZero, %setIndexVal
  %3 = load i32, i32* %valPtr
  %valSet = getelementptr i32, i32* %newVector, i32 %currIdx
  store i32 %3, i32* %valSet
  %idxInc = add i32 %currIdx, 1
  store i32 %idxInc, i32* %idxidxptr
  br label %indexCond

indexOut:                                         ; preds = %indexCond
  call void @printVec(i32* %newVector, i32 %numElem12)
  ret i32 0
}
