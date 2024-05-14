import telebot
from telebot import types
import random
import string

TOKEN = 'your-token:'

bot = telebot.TeleBot(TOKEN)

# Словарь для хранения выбранных пользователем параметров записи
user_data = {}

# словарь, который содержит дни недели в качестве ключей
schedule = {
    "Понедельник": {
        "9": 'free',
        "10": 'free',
        "11": 'busy',
        "12": 'free',
        "13": 'busy',
        "14": 'free',
        "15": 'free',
        "16": 'free',
        "17": 'busy',
        "18": 'free',
        "19": 'free',
        "20": 'free',
        "21": 'free'
    },
    "Вторник": {
        "9": 'free',
        "10": 'free',
        "11": 'busy',
        "12": 'free',
        "13": 'busy',
        "14": 'free',
        "15": 'free',
        "16": 'busy',
        "17": 'free',
        "18": 'free',
        "19": 'free',
        "20": 'free',
        "21": 'free'
    },
    "Среда": {
        "9": 'free',
        "10": 'free',
        "11": 'busy',
        "12": 'free',
        "13": 'busy',
        "14": 'free',
        "15": 'free',
        "16": 'free',
        "17": 'free',
        "18": 'free',
        "19": 'free',
        "20": 'free',
        "21": 'free'
    },
    "Четверг": {
        "9": 'free',
        "10": 'free',
        "11": 'busy',
        "12": 'free',
        "13": 'busy',
        "14": 'free',
        "15": 'free',
        "16": 'free',
        "17": 'free',
        "18": 'free',
        "19": 'free',
        "20": 'free',
        "21": 'free'
    },
    "Пятница": {
        "9": 'free',
        "10": 'free',
        "11": 'busy',
        "12": 'free',
        "13": 'busy',
        "14": 'free',
        "15": 'free',
        "16": 'free',
        "17": 'free',
        "18": 'free',
        "19": 'free',
        "20": 'free',
        "21": 'free'
    }
}


# Собственная функция генерации случайной строки для кода бронирования
# Эта функция генерирует случайную строку заданной длины, состоящую из букв (в верхнем и нижнем регистре) и цифр.
def generate_barcode(length=8):
    letters_and_digits = string.ascii_letters + string.digits
    return ''.join(random.choice(letters_and_digits) for _ in range(length))


# Функция для чтения расписания и проверки доступности времени
def read_schedule(day, time):
    if day in schedule and time in schedule[day]:
        if schedule[day][time] == 'free':
            return f"Время {time} на {day} доступно для записи."
        else:
            return f"Время {time} на {day} уже занято. Пожалуйста, выберите другое время."
    else:
        return "Некорректный день или время."


# Функция для обновления расписания
def update_schedule(day, time):
    if day in schedule and time in schedule[day]:
        if schedule[day][time] == 'free':
            schedule[day][time] = 'busy'
            return f"Вы успешно записались на {time} в {day}."
        else:
            return f"Извините, время {time} на {day} уже занято. Попробуйте выбрать другое время."
    else:
        return "Некорректный день или время."


# Обработчик команды /start
# Отправляется приветственное сообщения с кнопкой "Записаться"
@bot.message_handler(commands=['start'])
def send_welcome(message):
    # Получаем имя пользователя из объекта message
    user_name = message.from_user.first_name
    markup = types.ReplyKeyboardMarkup(one_time_keyboard=True, resize_keyboard=True)  # Уменьшаем размер клавиатуры
    markup.add(types.KeyboardButton('Записаться'))
    bot.reply_to(message, f"Здравствуйте, {user_name}! Этот бот поможет Вам записаться к нам. "
                          "Нажмите кнопку <Записаться> для начала бронирования.", reply_markup=markup)


# Обработчик для слова "Записаться" в сообщении
@bot.message_handler(func=lambda message: message.text.lower() == "записаться")
def ask_master(message):
    markup = types.ReplyKeyboardMarkup(one_time_keyboard=True, resize_keyboard=True)
    # пользователю предлагается выбрать мастера, нажав на соответствующую кнопку
    markup.add(types.KeyboardButton('Анна'), types.KeyboardButton('Мария'))
    bot.send_message(message.chat.id, "Выберите мастера", reply_markup=markup)
    bot.register_next_step_handler(message, save_master)


# Функция для сохранения выбранного мастера и перехода к выбору услуги
def save_master(message):
    user_choice = message.text.lower()
    # если мастер на предыдущем шаге был выбран верно
    if user_choice == 'анна' or user_choice == 'мария':
        user_data['master'] = user_choice.capitalize()
        markup = types.ReplyKeyboardMarkup(one_time_keyboard=True, resize_keyboard=True)
        # переход к выбору типа услуги
        markup.add(types.KeyboardButton('Стрижка'), types.KeyboardButton('Окрашивание'),
                   types.KeyboardButton('Наращивание'), types.KeyboardButton('Подравнивание'))
        bot.send_message(message.chat.id, f"Вы выбрали мастера {user_data['master']}. "
                                          "Теперь выберите тип услуги", reply_markup=markup)
        bot.register_next_step_handler(message, save_service)
    # если пользователь выбрал мастера, отличного от "Анна" или "Мария"
    else:
        # отправляется сообщение с просьбой выбрать правильного мастера
        bot.send_message(message.chat.id, "Пожалуйста, выберите мастера Анна или Мария.")
        bot.register_next_step_handler(message, save_master)


# Функция для сохранения выбранной услуги и перехода к выбору дня
def save_service(message):
    # Сохраняем выбранную пользователем услугу
    user_choice = message.text.lower()
    if user_choice in ['стрижка', 'окрашивание', 'наращивание', 'подравнивание']:
        user_data['service'] = user_choice.capitalize()
        markup = types.ReplyKeyboardMarkup(one_time_keyboard=True, resize_keyboard=True)
        for day in schedule:  # We don't need to read schedule here
            if day != "День недели":
                markup.add(types.KeyboardButton(day))
        # Пользователю предлагается выбрать день для записи
        bot.send_message(message.chat.id, "Выберите день", reply_markup=markup)
        bot.register_next_step_handler(message, save_day)
    else:
        # Если услуга не распознана, просим пользователя выбрать из списка
        bot.send_message(message.chat.id,
                         "Пожалуйста, выберите тип услуги: Стрижка, Окрашивание, Наращивание или Подравнивание.")
        bot.register_next_step_handler(message, save_service)


# Функция для сохранения выбранного дня и перехода к выбору времени
def save_day(message):
    # Сохраняем выбранный пользователем день
    user_choice = message.text
    if user_choice in schedule:
        user_data['day'] = user_choice
        markup = types.ReplyKeyboardMarkup(one_time_keyboard=True, resize_keyboard=True)
        for hour, slot in schedule[user_choice].items():
            if slot == 'free':
                markup.add(types.KeyboardButton(hour))
        # Пользователю предлагается выбрать время для записи
        bot.send_message(message.chat.id, "Выберите время", reply_markup=markup)
        bot.register_next_step_handler(message, save_time)
    else:
        # Если день не распознан, просим пользователя выбрать из списка
        bot.send_message(message.chat.id, "Пожалуйста, выберите день из предложенных вариантов.")
        bot.register_next_step_handler(message, save_day)


# Функция для сохранения выбранного времени и перехода к подтверждению
def save_time(message):
    user_choice = message.text
    if user_choice.isdigit() and 9 <= int(user_choice) <= 21:
        user_data['time'] = user_choice
        user_name = message.from_user.first_name
        # cохраняем имя пользователя
        user_data['user_name'] = user_name
        day = user_data['day']
        time = user_data['time']
        master = user_data['master']
        service = user_data['service']

        # Генерация случайного кода бронирования
        random_barcode = generate_barcode()

        # Записываются данные в файл
        with open("booking_log.txt", "a", encoding="utf-8") as file:
            file.write(
                f"Имя пользователя: {user_name}, Дата: {day}, Мастер: {master}, Услуга: {service}, Время: {time}\n")

        # Обновляем расписание в словаре
        update_schedule(day, time)

        # Отправляем сообщение об успешной записи и генерируемый код бронирования
        bot.send_message(message.chat.id,
                         f"Спасибо, {user_name}! Вы успешно записаны на процедуру {service}, {day} в {time}:00. "
                         f"Ваш код для подтверждения бронирования: {random_barcode}. "
                         f"Хорошего дня!")
    else:
        # Если введенное значение времени не соответствует условиям, просим пользователя ввести корректное время
        bot.send_message(message.chat.id, "Пожалуйста, выберите корректное время с 9 до 21.")
        bot.register_next_step_handler(message, save_time)


# обрабатывает ошибки ввода,
# направляет пользователя на начало бронирования
@bot.message_handler(func=lambda message: True)
def echo_all(message):
    bot.reply_to(message, "Простите, я не понял вашего сообщения. "
                          "Для начала бронирования напишите Записаться.")


bot.polling()

