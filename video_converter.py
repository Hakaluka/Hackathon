import cv2
import os


current_directory = os.getcwd()

def images_to_video(image_folder, video_name, fps):
    images = [img for img in sorted(os.listdir(current_directory+ '/screenshot')) if img.endswith((".png", ".jpg", ".jpeg", ".bmp"))]
    if not images:
        raise ValueError("No images found in the specified folder")

    first_image_path = os.path.join(image_folder, images[0])
    frame = cv2.imread(first_image_path)
    
    # Check if the first frame is loaded correctly
    if frame is None:
        raise ValueError(f"Unable to read the first image: {first_image_path}")

    height, width, layers = frame.shape

    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    video = cv2.VideoWriter(video_name, fourcc, fps, (width, height))

    for image in images:
        image_path = os.path.join(image_folder, image)
        frame = cv2.imread(image_path)

        if frame is None:
            print(f"Warning: Unable to read image {image_path}. Skipping.")
            continue

        video.write(frame)

    video.release()

# Example usage
image_folder = current_directory+ '/screenshot'
video_name = 'output_video.avi'
fps = 28 # Frames per second

images_to_video(image_folder, video_name, fps)
import shutil; shutil.rmtree(current_directory+ "\\screenshot\\")
