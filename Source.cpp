#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <thread>


#define SCALE 2
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define FIELD_WIDTH WINDOW_WIDTH / SCALE
#define FIELD_HEIGHT WINDOW_HEIGHT / SCALE
#define FPS_LOCK 36000

class Point {
public:
    double x=0, y=0;
    double v_x=0, v_y=0;
    double R = 32, m = 0;   //radius in pixels
    sf::Color color = sf::Color::White;
    Point(double _x, double _y) {
        x = _x; y = _y;
    }
    Point(double _x, double _y, double _v_x,double _v_y, double _R = 32) {
        x = _x; y = _y;
        v_x = _v_x; v_y = _v_y;
        R = _R;
        m = acos(-1) * R * R;
    }
    void calculateMoving(const double& dt) {
        double velocity_mag = sqrt(v_x * v_x + v_y * v_y);
        if (velocity_mag < 1e-02 && fabs(y - WINDOW_HEIGHT + R) < 1e-02) return;

        if (x + R > WINDOW_WIDTH) {
            x = WINDOW_WIDTH - R;
            v_x *= -0.75;
            v_y *= 0.75;
        }
        if (x - R < 0) {
            x = R;
            v_x *= -0.75;
            v_y *= 0.75;
        }
        if (y + R > WINDOW_HEIGHT) {
            y = WINDOW_HEIGHT - R;
            v_y *= -0.75;
            v_x *= 0.75;
        }
        if (y - R < 0) {
            y = R;
            v_y *= -0.75;
            v_x *= 0.75;
        }
        v_y -= 0.0005 * v_y * velocity_mag * dt;
        v_x -= 0.0005 * v_x * velocity_mag * dt;
        v_y += dt * 9.8;
        x += v_x * dt;
        y += v_y * dt;
        

        
       
      //  std::cout << "Magnitude: " << velocity_mag << std::endl;
       
 
    }

};


int main()
{
    srand(0);
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML works!", sf::Style::Default, settings);
    sf::CircleShape shape(32.f, 150);
    sf::Texture texture;
    sf::Clock clock;

    std::chrono::microseconds fps_lock_time_per_frame = round<std::chrono::microseconds>(std::chrono::duration<float>{1.f/FPS_LOCK});;

    if (!texture.loadFromFile("pic.png"))
    {
        std::cout << "Cant load texture!" << std::endl;
        exit(0);
    }
   // shape.setTexture(&texture); 
  //  shape.setOrigin(32.f, 32.f);


    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    std::vector <sf::CircleShape> shapes = { shape };
    std::vector <sf::RectangleShape> r_shapes;
    std::vector <sf::CircleShape> tri_shapes;
    std::vector <Point> points;
  
    int _x, _y; //x,y for creating circles
    bool isReleased = true, isPaused = false;

    while (window.isOpen())
    {
        //FPS METER
        end = std::chrono::system_clock::now();
        std::chrono::duration<float> diff = end - start;
        if (diff < fps_lock_time_per_frame) {
            std::this_thread::sleep_for(fps_lock_time_per_frame - diff);
            diff = fps_lock_time_per_frame + diff;
        }       
        window.setTitle("Points simulator v.0.1 by Yaroslav Starikov. " + std::to_string(int(1.0f / diff.count())) + " fps");
        start = std::chrono::system_clock::now();

        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
             //   isPaused = !isPaused;
            }

            if (event.type == sf::Event::MouseWheelScrolled) {}

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (isReleased) {
                    _x = event.mouseButton.x;
                    _y = event.mouseButton.y;
                    isReleased = false;
                }          
             



              /*  if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    shape.setPosition(sf::Vector2f(_x, _y)); // absolute position
                    shapes.push_back(shape);
                }
                else {
                    shape.setPosition(sf::Vector2f(_x, _y)); // absolute position
                    shapes.push_back(shape);
                    shapes.back().scale(2.0, 2.0);
                }*/
            }
            if (event.type == sf::Event::MouseMoved) {
                if (!isReleased) {
                    r_shapes.clear();
                    tri_shapes.clear();
                    sf::RectangleShape line(sf::Vector2f(sqrt(pow(event.mouseMove.x - _x, 2) + pow(event.mouseMove.y - _y, 2)), 5));
                    sf::CircleShape arrow(15,3);
                    arrow.setOrigin(arrow.getRadius(), arrow.getRadius());
                    line.setOrigin(0, 2.5);
                    line.setPosition(sf::Vector2f(_x, _y));
                    arrow.setPosition(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                    double angle = 0;
                    if (event.mouseMove.x - _x != 0) {
                        angle = atan(static_cast<double>(event.mouseMove.y - _y) / fabs(event.mouseMove.x - _x)) * 180 / acos(-1);
                        if (event.mouseMove.x - _x < 0)
                            angle = 180 - angle;
                    }
                    else {
                        angle = event.mouseMove.y - _y > 0 ? 90 : -90;
                    }
                    line.setFillColor(sf::Color::Blue);
                    arrow.setFillColor(sf::Color::Blue);
                    line.rotate(angle);
                    arrow.rotate(angle + 90);
                    tri_shapes.push_back(arrow);
                    r_shapes.push_back(line);
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                int _x_end = event.mouseButton.x, _y_end = event.mouseButton.y;
                isReleased = true;
                points.push_back(Point(_x, _y, 3 * static_cast<int>(_x_end - _x), 3 * static_cast<int>(_y_end - _y), rand() % 90 + 10));
                points.back().color = sf::Color(rand() % 255, rand() % 255, rand() % 255);
    
                r_shapes.clear();
                tri_shapes.clear();
            }
        }
        //Calculations
        window.clear(sf::Color::White);
        double dt = clock.restart().asSeconds();
        if (!isPaused) {
            for (size_t i = 0; i < points.size(); ++i) {
               

                for (size_t j = i + 1; j < points.size(); ++j) {
                    double distance = sqrt(pow(points[i].x - points[j].x, 2) + pow(points[i].y - points[j].y, 2));
                    if (distance < points[i].R + points[j].R) {
                        double scalar1 = (points[i].x - points[j].x) * (points[i].v_x - points[j].v_x) + (points[i].y - points[j].y) * (points[i].v_y - points[j].v_y);
                        double scalar2 = (points[j].x - points[i].x) * (points[j].v_x - points[i].v_x) + (points[j].y - points[i].y) * (points[j].v_y - points[i].v_y);


                        double vx_new_i = points[i].v_x - 2 * points[j].m / (points[j].m + points[i].m) * scalar1 / distance / distance * (points[i].x - points[j].x),
                            
                            
                             vx_new_j= points[j].v_x - 2 * points[i].m / (points[j].m + points[i].m) * scalar2 / distance / distance * (points[j].x - points[i].x),
                                
                                
                                
                                vy_new_i= points[i].v_y - 2 * points[j].m / (points[j].m + points[i].m) * scalar1 / distance / distance * (points[i].y - points[j].y),
                                
                                
                                vy_new_j = points[j].v_y - 2 * points[i].m / (points[j].m + points[i].m) * scalar2 / distance / distance * (points[j].y - points[i].y);


                        points[i].v_x = vx_new_i;
                        points[i].v_y = vy_new_i;
                        points[j].v_x = vx_new_j;
                        points[j].v_y = vy_new_j;
                        
                        double shift = - distance + points[i].R + points[j].R;

                        points[i].x += shift / 2 * (points[i].x - points[j].x) / distance;
                        points[i].y += shift / 2 * (points[i].y - points[j].y) / distance;
                        points[j].x += -shift / 2 * (points[i].x - points[j].x) / distance;
                        points[j].y += -shift / 2 * (points[i].y - points[j].y) / distance;

                    }


                }
                points[i].calculateMoving(1e-03);
            }
        }
        for (Point& it : points) {
            
            shape.setRadius(it.R);
            shape.setOrigin(shape.getRadius(), shape.getRadius());
            shape.setPosition(sf::Vector2f(it.x, it.y)); // absolute position
            shape.setFillColor(it.color);

            shape.setOutlineThickness(3.f);
            shape.setOutlineColor(sf::Color::Black);
            window.draw(shape);
        }
        for (sf::RectangleShape& it : r_shapes) {
            window.draw(it);
        }
        for (sf::CircleShape& it : tri_shapes) {
            window.draw(it);
        }
        window.display();
    }

    return 0;
}