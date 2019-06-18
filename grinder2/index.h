const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <title>Lelit PL043MMI</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <link rel="shortcut icon" type="image/x-icon"
        href="data:image/png;base64,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">
    <link rel="apple-touch-icon" type="image/x-icon"
        href="data:image/png;base64,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">
    <style>
        form {
            border: 3px solid #f1f1f1;
        }

        input[type=number] {
            width: 100%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            box-sizing: border-box;
        }

        button {
            background-color: #4CAF50;
            color: white;
            padding: 14px 20px;
            margin: 8px 0;
            border: none;
            cursor: pointer;
            width: 100%;
        }

        .container {
            padding: 16px;
        }

        span.coffe2 {
            float: right;
            padding-top: 16px;
        }

        @media screen and (max-width: 300px) {
            span.coffe2 {
                display: block;
                float: none;
            }
        }
    </style>
</head>

<body>
    <form action="/submit">
        <div class="container">

            <label>
                <input type="radio" name="selectedcoffe" value="1" __checked1__>
                <b>Seconds for 7 gr.</b>
            </label>
            <input type="number" name="coffe1" value="__coffe1__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="2" __checked2__>
                <b>Seconds for 9 gr.</b>
            </label>
            <input type="number" name="coffe2" value="__coffe2__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="3" __checked3__>
                <b>Seconds for 12 gr.</b>
            </label>
            <input type="number" name="coffe3" value="__coffe3__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="4" __checked4__>
                <b>Seconds for 14 gr.</b>
            </label>
            <input type="number" name="coffe4" value="__coffe4__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="5" __checked5__>
                <b>Seconds for 15 gr.</b>
            </label>
            <input type="number" name="coffe5" value="__coffe5__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="6" __checked6__>
                <b>Seconds for 16 gr.</b>
            </label>
            <input type="number" name="coffe6" value="__coffe6__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="7" __checked7__>
                <b>Seconds for 17 gr.</b>
            </label>
            <input type="number" name="coffe7" value="__coffe7__" min="1" max="25" step="0.1" required>

            <label>
                <input type="radio" name="selectedcoffe" value="8" __checked8__>
                <b>Seconds for 18 gr.</b>
            </label>
            <input type="number" name="coffe8" value="__coffe8__" min="1" max="25" step="0.1" required>

            <button type="submit">Save</button>
        </div>
    </form>
</body>

</html>
)=====";
