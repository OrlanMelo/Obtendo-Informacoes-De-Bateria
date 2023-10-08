#include <Windows.h>
#include <batclass.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <ioapiset.h>
#include <string>
#include <iostream>
#pragma comment(lib,"setupapi.lib")

using namespace std;

class cFuncoes
{
private:

	HDEVINFO Result;
	bool InicializarClasseDeBateria()
	{
		bool bRetorno = false;

		Result = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (Result == INVALID_HANDLE_VALUE)
			cout << "Identificador inválido para a busca selecionada.." << GetLastError();
		else
		{
			bRetorno = true;
		}

		return bRetorno;
	}

	HANDLE Bateria;

public:

	void EnumerarBaterias()
	{
		if (InicializarClasseDeBateria() == true)
		{
			//Vai enumerar apenas 1 bateria.
			for (int i = 0; i < 1; i++)
			{
				SP_DEVICE_INTERFACE_DATA Informacao;
				Informacao.cbSize = sizeof(Informacao);

				SetupDiEnumDeviceInterfaces(Result, 0, &GUID_DEVCLASS_BATTERY, i, &Informacao);

				DWORD Tamanho;
				SetupDiGetDeviceInterfaceDetail(Result, &Informacao, 0, 0, &Tamanho, 0);

				SP_DEVICE_INTERFACE_DETAIL_DATA_W Data;
				Data.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

				BOOL bResult;
				bResult = SetupDiGetDeviceInterfaceDetail(Result, &Informacao, &Data, Tamanho, &Tamanho, 0);
				if (bResult == TRUE)
				{
					Bateria = CreateFile(Data.DevicePath, GENERIC_READ, FILE_SHARE_READ,
						0, OPEN_EXISTING, 0, 0);
					if (Bateria == INVALID_HANDLE_VALUE)
						cout << "Identificador inválido para o tipo de operação selecionada..";
					else
						ObterInformacoesDeBateria();
				}
				else
				{
					cout << "Ocorreu um erro durante a operação de busca.." << GetLastError();
				}
			}
		}
	}

	void ObterInformacoesDeBateria()
	{
		BATTERY_QUERY_INFORMATION Informacoes;
		BATTERY_INFORMATION Informacoes2;

		DWORD BytesRetornados;
		DeviceIoControl(Bateria, IOCTL_BATTERY_QUERY_TAG, 0, 0, &Informacoes.BatteryTag, sizeof(Informacoes.BatteryTag), &BytesRetornados, 0);
		Informacoes.InformationLevel = BatteryInformation;

		DeviceIoControl(Bateria, IOCTL_BATTERY_QUERY_INFORMATION, &Informacoes, sizeof(Informacoes), &Informacoes2, sizeof(Informacoes2), &BytesRetornados,
			0);

		if (Informacoes2.Capabilities == BATTERY_CAPACITY_RELATIVE)
			cout << "Capacidade relativa..\n";
		else if (Informacoes2.Capabilities == BATTERY_SYSTEM_BATTERY)
			cout << "Esta bateria pode fornecer energia geral para o sistema..\n";

		BATTERY_WAIT_STATUS OutrasInformacoes;
		OutrasInformacoes.BatteryTag = Informacoes.BatteryTag;

		BATTERY_STATUS Status;
		DeviceIoControl(Bateria, IOCTL_BATTERY_QUERY_STATUS, &OutrasInformacoes, sizeof(OutrasInformacoes), &
			Status, sizeof(Status), &BytesRetornados, 0);

		if (Status.PowerState & BATTERY_POWER_ON_LINE)
			cout << "O computador está em uma conexão direta com a fonte, sem descarregar a bateria.\n";
		else if (Status.PowerState & BATTERY_CHARGING)
			cout << "A bateria está carregando..\n";
		else if (Status.PowerState & BATTERY_DISCHARGING)
			cout << "A bateria está descarregando..\n";
		else
			cout << "A bateria precisa de sua atenção, pode estar com mal funcionamento..\n";

		if (Informacoes2.Technology == 0)
			cout << "Bateria não recarregável, alcalina.\n";
		else
			cout << "Bateria recarregável.\n";

		cout << "Substância da bateria: " << Informacoes2.Chemistry << '\n';

		long Calcular1 = (double)Status.Capacity / Informacoes2.FullChargedCapacity * 1000;
		cout << "Capacidade da bateria em kWh ( Carregada 100% ): " << Calcular1 << " kWh.\n";

		if (Informacoes2.CycleCount == 0)
			cout << "Ciclos de carga: Não disponível.\n";
		else
			cout << "Ciclos de carga: " << Informacoes2.CycleCount << " ciclos.\n";

		//Use alguma cálculadora ciêntifica para converter ( mWh ) Megawatt-hora para ( kWh ) Quilowatt-hora.
		//Este cálculo obtém apenas o valor em ( mWh ).
		long Calcular = (double)Status.Capacity / Informacoes2.FullChargedCapacity;
		cout << "Capacidade: " << Calcular << " mWh. ( Megawatt-hora ).\n";

		if (Status.Voltage == BATTERY_UNKNOWN_VOLTAGE)
		{
			cout << "Voltagem desconhecida..\n";
		}
		else
			cout << "Voltagem: " << Status.Voltage << " mv ( Millivolts )\n";

		system("pause");
	}

}Funcoes;

int main()
{

	cout << "O assistente está executando operações de busca de informações sobre bateria...\n\n";

	Funcoes.EnumerarBaterias();

	//system("pause");
}