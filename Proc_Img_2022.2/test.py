import cv2
import pytesseract


def preprocess_image(img):
    """Applies image pre-processing steps to improve OCR results"""

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    thresh = cv2.threshold(
        gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)[1]
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
    dilation = cv2.dilate(thresh, kernel, iterations=1)

    return dilation


def locate_text(image):
    """Locates text regions in the image"""

    contours, hierarchy = cv2.findContours(
        image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    boxes = [cv2.boundingRect(c) for c in contours]

    return boxes


def segment_characters(image, boxes):
    """Segments characters from the text regions"""

    characters = []

    for box in boxes:
        x, y, w, h = box

        if w == 0 or h == 0:
            continue

        char_img = image[y:y+h, x:x+w]
        characters.append(char_img)

    return characters


def recognize_characters(characters):
    """Recognizes characters from the segmented images"""

    recognized_characters = []

    for char in characters:
        char_text = pytesseract.image_to_string(char, config='--psm 10')

        if char_text.strip():
            recognized_characters.append(char_text)

    return recognized_characters


def recognize_words(img):
    """Recognizes words from the original image"""

    text = pytesseract.image_to_string(img)
    words = text.split()

    return words


def postprocess_text(recognized_characters, recognized_words):
    """Post-processes the recognized characters and words"""

    recognized_text = recognized_characters + recognized_words
    recognized_text = sorted(list(set(recognized_text)))
    complete_text = ' '.join(recognized_text)

    return complete_text


img = cv2.imread('/home/mtsamorim/Documents/compgraf/test3.png')
processed_image = preprocess_image(img)
boxes = locate_text(processed_image)
characters = segment_characters(processed_image, boxes)
recognized_characters = recognize_characters(characters)
recognized_words = recognize_words(img)
complete_text = postprocess_text(recognized_characters, recognized_words)

if recognized_words:
    print("Recognized words: ", recognized_words)

# Recognize words
recognized_words = recognize_words(img)

# Concatenate recognized words
complete_text = ' '.join(recognized_words)

# Print complete text
if complete_text:
    print("Complete text: ", complete_text)

# Draw blue rectangles around each character on the original image
for box in boxes:
    x, y, w, h = box
    cv2.rectangle(img, (x, y), (x+w, y+h), (255, 0, 0), 2)

# Export the treated image and an image with a rectangle around each character recognized
cv2.imwrite('processed_image.jpg', processed_image)
cv2.imwrite('recognized_characters.jpg', img)
