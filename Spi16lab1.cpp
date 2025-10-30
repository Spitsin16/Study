#include <iostream>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

// Структура, представляющая сегмент трубопровода
struct PipelineSegment {
    string kmMark;
    double lengthKm = 0.0;
    double diameterMm = 0.0;
    bool isUnderRepair = false;
};

// Структура, представляющая компрессорную станцию
struct CompressorStation {
    string name;
    int totalWorkshops = 0;
    int activeWorkshops = 0;
    int classLevel = 0;
};

// Шаблонная функция для безопасного ввода с проверкой
template <typename T>
T getValidInput(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Неверный ввод. \n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Отображение главного меню
void showMenu() {
    cout << "\n   СИСТЕМА УПРАВЛЕНИЯ ТРУБОПРОВОДОМ\n";
    cout << "----------------------------------------\n";
    cout << "1. Добавить сегмент трубопровода\n";
    cout << "2. Добавить компрессорную станцию\n";
    cout << "3. Просмотреть все объекты\n";
    cout << "4. Изменить статус ремонта трубы\n";
    cout << "5. Управление цехами станции\n";
    cout << "6. Сохранить все данные\n";
    cout << "7. Загрузить данные\n";
    cout << "0. Выход\n";
    cout << "----------------------------------------\n";
}

// Создание нового сегмента трубопровода
void addPipeline(PipelineSegment& pipe) {
    cout << "\n--- Добавление нового сегмента трубопровода ---\n";
    cout << "Введите километровую отметку: ";
    getline(cin, pipe.kmMark);

    pipe.lengthKm = getValidInput<double>("Введите длину (км): ");
    while (pipe.lengthKm <= 0) {
        cout << "Длина должна быть положительной. ";
        pipe.lengthKm = getValidInput<double>("Введите длину (км): ");
    }

    pipe.diameterMm = getValidInput<double>("Введите диаметр (мм): ");
    while (pipe.diameterMm <= 0) {
        cout << "Диаметр должен быть положительным. ";
        pipe.diameterMm = getValidInput<double>("Введите диаметр (мм): ");
    }

    pipe.isUnderRepair = false;
    cout << "Сегмент трубопровода успешно добавлен!\n";
}

// Создание новой компрессорной станции
void addStation(CompressorStation& station) {
    cout << "\n--- Добавление новой компрессорной станции ---\n";
    cout << "Введите название станции: ";
    getline(cin, station.name);

    station.totalWorkshops = getValidInput<int>("Введите общее количество цехов: ");
    while (station.totalWorkshops <= 0) {
        cout << "Общее количество цехов должно быть положительным. ";
        station.totalWorkshops = getValidInput<int>("Введите общее количество цехов: ");
    }

    station.activeWorkshops = getValidInput<int>("Введите количество активных цехов: ");
    while (station.activeWorkshops < 0 || station.activeWorkshops > station.totalWorkshops) {
        cout << "Активные цехи не могут превышать общее количество. ";
        station.activeWorkshops = getValidInput<int>("Введите количество активных цехов: ");
    }

    station.classLevel = getValidInput<int>("Введите класс станции (1-5): ");
    while (station.classLevel < 1 || station.classLevel > 5) {
        cout << "Класс должен быть от 1 до 5. ";
        station.classLevel = getValidInput<int>("Введите класс станции (1-5): ");
    }

    cout << "Компрессорная станция успешно добавлена!\n";
}

// Отображение всех сохраненных объектов
void displayObjects(const PipelineSegment& pipe, const CompressorStation& station) {
    cout << "\n--- ТЕКУЩИЕ ОБЪЕКТЫ ---\n";

    cout << "\nСЕГМЕНТ ТРУБОПРОВОДА:\n";
    if (pipe.kmMark.empty()) {
        cout << "  Данные отсутствуют\n";
    } else {
        cout << "  Километровая отметка: " << pipe.kmMark << "\n";
        cout << "  Длина: " << pipe.lengthKm << " км\n";
        cout << "  Диаметр: " << pipe.diameterMm << " мм\n";
        cout << "  Статус ремонта: " << (pipe.isUnderRepair ? "На ремонте" : "Рабочий") << "\n";
    }

    cout << "\nКОМПРЕССОРНАЯ СТАНЦИЯ:\n";
    if (station.name.empty()) {
        cout << "  Данные отсутствуют\n";
    } else {
        cout << "  Название: " << station.name << "\n";
        cout << "  Цехи: " << station.activeWorkshops << " / " << station.totalWorkshops << "\n";
        cout << "  Класс: " << station.classLevel << "\n";
    }
}

// Изменение статуса ремонта трубопровода
void toggleRepairStatus(PipelineSegment& pipe) {
    if (pipe.kmMark.empty()) {
        cout << "Ошибка: Сегмент трубопровода не существует. Сначала добавьте его.\n";
        return;
    }

    cout << "\nТекущий статус: " << (pipe.isUnderRepair ? "На ремонте" : "Рабочий") << "\n";
    cout << "1. Отметить как находящийся на ремонте\n";
    cout << "2. Отметить как рабочий\n";

    int choice = getValidInput<int>("Выберите опцию: ");
    if (choice == 1) {
        pipe.isUnderRepair = true;
        cout << "Трубопровод отмечен как находящийся на ремонте.\n";
    } else if (choice == 2) {
        pipe.isUnderRepair = false;
        cout << "Трубопровод отмечен как рабочий.\n";
    } else {
        cout << "Неверная опция.\n";
    }
}

// Управление активными цехами на станции
void manageWorkshops(CompressorStation& station) {
    if (station.name.empty()) {
        cout << "Ошибка: Компрессорная станция не существует. Сначала добавьте ее.\n";
        return;
    }

    cout << "\nТекущие цехи: " << station.activeWorkshops << " / " << station.totalWorkshops << "\n";
    cout << "1. Запустить цех\n";
    cout << "2. Остановить цех\n";

    int action = getValidInput<int>("Выберите действие: ");
    if (action == 1) {
        if (station.activeWorkshops < station.totalWorkshops) {
            station.activeWorkshops++;
            cout << "Цех запущен. Сейчас активно " << station.activeWorkshops << " цехов.\n";
        } else {
            cout << "Все цехи уже активны.\n";
        }
    } else if (action == 2) {
        if (station.activeWorkshops > 0) {
            station.activeWorkshops--;
            cout << "Цех остановлен. Сейчас активно " << station.activeWorkshops << " цехов.\n";
        } else {
            cout << "Нет активных цехов для остановки.\n";
        }
    } else {
        cout << "Неверная опция.\n";
    }
}

// Сохранение данных трубопровода в выходной поток
void savePipe(const PipelineSegment& pipe, ofstream& out) {
    out << "PIPE\n";
    out << pipe.kmMark << "\n";
    out << pipe.lengthKm << "\n";
    out << pipe.diameterMm << "\n";
    out << pipe.isUnderRepair << "\n";
}

// Сохранение данных станции в выходной поток
void saveStation(const CompressorStation& station, ofstream& out) {
    out << "STATION\n";
    out << station.name << "\n";
    out << station.totalWorkshops << "\n";
    out << station.activeWorkshops << "\n";
    out << station.classLevel << "\n";
}

// Сохранение всех данных в один файл
void saveAll(const PipelineSegment& pipe, const CompressorStation& station) {
    ofstream file("pipeline_data.txt");
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось открыть файл для записи.\n";
        return;
    }

    if (!pipe.kmMark.empty()) {
        savePipe(pipe, file);
    }
    if (!station.name.empty()) {
        saveStation(station, file);
    }

    file.close();
    cout << "Данные успешно сохранены в pipeline_data.txt\n";
}

// Загрузка данных трубопровода из входного потока
void loadPipe(PipelineSegment& pipe, ifstream& in) {
    getline(in, pipe.kmMark);
    in >> pipe.lengthKm >> pipe.diameterMm >> pipe.isUnderRepair;
    
}

// Загрузка данных станции из входного потока
void loadStation(CompressorStation& station, ifstream& in) {
    getline(in, station.name);
    in >> station.totalWorkshops >> station.activeWorkshops >> station.classLevel;
    
}

// Загрузка всех данных из файла
void loadAll(PipelineSegment& pipe, CompressorStation& station) {
    ifstream file("pipeline_data.txt");
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось открыть файл для чтения.\n";
        return;
    }

    pipe.kmMark.clear();
    station.name.clear();

    string type;
    while (getline(file, type)) {
        if (type == "PIPE") {
            loadPipe(pipe, file);
        } else if (type == "STATION") {
            loadStation(station, file);
        }
    }

    file.close();
    cout << "Данные успешно загружены из pipeline_data.txt\n";
}

// Обработка выбора пользователя в меню
void processChoice(int choice, PipelineSegment& pipe, CompressorStation& station) {
    switch (choice) {
        case 1:
            addPipeline(pipe);
            break;
        case 2:
            addStation(station);
            break;
        case 3:
            displayObjects(pipe, station);
            break;
        case 4:
            toggleRepairStatus(pipe);
            break;
        case 5:
            manageWorkshops(station);
            break;
        case 6:
            saveAll(pipe, station);
            break;
        case 7:
            loadAll(pipe, station);
            break;
        case 0:
            cout << "Выход из программы.\n";
            exit(0);
        default:
            cout << "Неверный выбор. Пожалуйста, попробуйте снова.\n";
    }
}


int main() {
    PipelineSegment currentPipe;
    CompressorStation currentStation;

    while (true) {
        showMenu();
        int option = getValidInput<int>("Введите ваш выбор: ");
        processChoice(option, currentPipe, currentStation);
    }

    return 0;
}