void loopStrategy() {
  switch (strategy) {
    case 0: // 000
      loopStrategy0();
    break;
    case 1: // 001
      loopStrategy1();  
    break;
    case 2: // 010
      loopStrategy2();
    break;
    case 3: // 011
      loopStrategy3();
    break;
    case 4: // 100
      loopStrategy4();
    break;
    case 5: // 101
      //loopStrategy5();
    break;
    case 6: // 110
      //loopStrategy6();
    break;
    case 7: // 111
      //loopStrategy7();
    break;
  }
}