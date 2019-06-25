def calcolaPrecision(cospeciationCost,duplicationCost,hostswitchCost,sortingCost):
    precision=1;
        
    try:
        newPrecision = len([x.strip() for x in cospeciationCost.split('.')][1])
        if newPrecision > precision :
            precision = newPrecision;        
    except Exception:
        precision =precision;
        
    minPrecision =precision;

    try:
        newPrecision = len([x.strip() for x in duplicationCost.split('.')][1])
    except Exception:
        newPrecision =1;
    if newPrecision > precision :
        precision = newPrecision;
    if newPrecision < minPrecision :
        minPrecision = newPrecision;
        
    try:
        newPrecision = len([x.strip() for x in hostswitchCost.split('.')][1])
    except Exception:
        newPrecision =1;
    if newPrecision > precision :
        precision = newPrecision;
    if newPrecision < minPrecision :
        minPrecision = newPrecision;
        
    try:
        newPrecision = len([x.strip() for x in sortingCost.split('.')][1])
    except Exception:
        newPrecision =1;
    if newPrecision > precision :
        precision = newPrecision;
    if newPrecision < minPrecision :
        minPrecision = newPrecision;
        
        
    if( precision-minPrecision)>7 :
        precision = minPrecision+6;

    return precision;
