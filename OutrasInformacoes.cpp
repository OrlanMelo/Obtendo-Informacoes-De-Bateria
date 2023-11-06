#include <Windows.h>
#include <string>
#include <iostream>
#pragma comment(lib,"kernel32.lib")

using namespace std;

class cFuncoes
{
private:

	SYSTEM_POWER_STATUS Energia;

public:

	void ObterInformacoesDeBateria()
	{
		GetSystemPowerStatus(&Energia);
		
		if (Energia.ACLineStatus == 0)
		{
			cout << "Desconectado da fonte de energia..\n";
		}
		else
		{
			cout << "Conectado a fonte de energia..\n";
		}

		switch (Energia.BatteryFlag)
		{
		case 1:
			cout << "Carga: Praticamente carregada..\n";
			break;
		case 2:
			cout << "Carga: Baixa, descarregando..\n";
			break;
		case 4:
			cout << "Carga: A bateria precisa ser recarregada imediatamente..\n";
			break;
		case 8:
			cout << "Carga: Carregando..\n";
			break;
		default:
			break;
		}

		if (Energia.SystemStatusFlag == 0)
		{
			cout << "Economia: Desativada..\n";
		}
		else
		{
			cout << "Economia: Ativa..\n";
		}

		cout << "Percentual de bateria: " << Energia.BatteryLifePercent * 1 << "% restantes..\n";
	}

}Funcoes;

int main()
{

	cout << "O assistente está executando operações de busca de informações sobre bateria...\n\n";

	Funcoes.ObterInformacoesDeBateria();

	system("pause");
}
