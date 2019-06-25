import os
from eucaliptFolder import calcolaPrecision as calcolaPrecision

def generaFileEucalipt(file_name, name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, soluzioniRandomiche, maximumHostSwitchDistance):		
	precision = calcolaPrecision.calcolaPrecision(cospeciationCost,duplicationCost,hostswitchCost,sortingCost)
	#ELABORO CON EUCALIPT IL FILE NEXUS
	input_data = "./input_data/"+name+".nex"

	maximumHostSwitchDistanceCommand = ""
	if (maximumHostSwitchDistance != "NaN"):
		maximumHostSwitchDistanceCommand = " -j "+ maximumHostSwitchDistance
	maxSolution = ""
	soluzioniRandomicheCommand = ""
	if (number != "NaN"):
		maxSolution = " -n "+ number
		if ((soluzioniRandomiche == "true")) :
			soluzioniRandomicheCommand = " -random "
	rootToRootCommand = ""
	if ((rootToRoot == "true")) :
		rootToRootCommand = " -root "
	os.system("java -jar ./eucaliptFolder/Eucalypt-1.0.4.jar -i "+input_data+" -task 2 -o "+file_name+" -cc "+cospeciationCost+" -cd "+duplicationCost+" -ch "+hostswitchCost+" -cl "+sortingCost+" -p "+str(precision)+ maxSolution + rootToRootCommand + soluzioniRandomicheCommand + maximumHostSwitchDistanceCommand + "\n")


def pulisciServer(file_name):
	os.system("rm "+file_name );

def rimuoviCicli(file_name, metodoRimuviCicli):
	file_nameApp = ""+file_name+"BBBBBBBBBBBBBBBZZZZdddserraa"
	os.system("java -jar ./eucaliptFolder/Eucalypt-1.0.4.jar -i "+file_name+" -task 4 -test "+ metodoRimuviCicli +" -o "+file_nameApp+" \n");
	os.system("rm "+file_name );
	os.system("mv "+file_nameApp+" "+file_name );
	#os.system("rm "+file_nameApp );

