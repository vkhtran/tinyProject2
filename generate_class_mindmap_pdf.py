from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4, landscape
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import cm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import Paragraph, SimpleDocTemplate, Spacer, PageBreak
from reportlab.graphics.shapes import Drawing, Rect, String, Line
from reportlab.graphics import renderPDF


WORKDIR = Path(r"C:\Users\User\Desktop\Programming 2\Programming2\2026_Exercise\tinyproject")
OUTPUT = WORKDIR / "Project2_Class_MindMap.pdf"


def register_font():
    candidates = [
        Path(r"C:\Windows\Fonts\arial.ttf"),
        Path(r"C:\Windows\Fonts\segoeui.ttf"),
        Path(r"C:\Windows\Fonts\tahoma.ttf"),
    ]
    for font_path in candidates:
        if font_path.exists():
            pdfmetrics.registerFont(TTFont("MindMapFont", str(font_path)))
            return "MindMapFont"
    return "Helvetica"


FONT_NAME = register_font()


def styles():
    base = getSampleStyleSheet()
    base.add(
        ParagraphStyle(
            name="TitleMind",
            parent=base["Title"],
            fontName=FONT_NAME,
            fontSize=19,
            leading=24,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#12344D"),
            spaceAfter=8,
        )
    )
    base.add(
        ParagraphStyle(
            name="SubMind",
            parent=base["BodyText"],
            fontName=FONT_NAME,
            fontSize=10,
            leading=13,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#52606D"),
            spaceAfter=10,
        )
    )
    base.add(
        ParagraphStyle(
            name="HeadMind",
            parent=base["Heading2"],
            fontName=FONT_NAME,
            fontSize=13,
            leading=16,
            alignment=TA_LEFT,
            textColor=colors.HexColor("#12344D"),
            spaceBefore=8,
            spaceAfter=4,
        )
    )
    base.add(
        ParagraphStyle(
            name="BodyMind",
            parent=base["BodyText"],
            fontName=FONT_NAME,
            fontSize=9.5,
            leading=13,
            alignment=TA_LEFT,
            spaceAfter=5,
        )
    )
    return base


STYLES = styles()


def add_page_number(canvas, doc):
    canvas.setFont(FONT_NAME, 9)
    canvas.setFillColor(colors.HexColor("#7B8794"))
    canvas.drawRightString(doc.pagesize[0] - 1.4 * cm, 1.0 * cm, f"Page {doc.page}")


def draw_box(drawing, x, y, w, h, title, fill="#D6F5E3", title_fill="#12344D", lines=None):
    lines = lines or []
    drawing.add(
        Rect(
            x,
            y,
            w,
            h,
            rx=10,
            ry=10,
            fillColor=colors.HexColor(fill),
            strokeColor=colors.HexColor("#12344D"),
            strokeWidth=1.4,
        )
    )
    drawing.add(
        String(
            x + 8,
            y + h - 18,
            title,
            fontName=FONT_NAME,
            fontSize=11,
            fillColor=colors.HexColor(title_fill),
        )
    )
    text_y = y + h - 34
    for line in lines[:6]:
        drawing.add(
            String(
                x + 10,
                text_y,
                line,
                fontName=FONT_NAME,
                fontSize=8,
                fillColor=colors.black,
            )
        )
        text_y -= 11


def connect(drawing, x1, y1, x2, y2, label=""):
    drawing.add(Line(x1, y1, x2, y2, strokeColor=colors.black, strokeWidth=2.0))
    if label:
        mx = (x1 + x2) / 2
        my = (y1 + y2) / 2
        drawing.add(
            String(
                mx + 4,
                my + 4,
                label,
                fontName=FONT_NAME,
                fontSize=8,
                fillColor=colors.HexColor("#52606D"),
            )
        )


def relationship_map():
    d = Drawing(760, 500)

    draw_box(d, 40, 355, 120, 72, "User", "#2DD4BF", lines=[
        "id", "username", "role", "displayMenu()"
    ])
    draw_box(d, 240, 420, 140, 62, "Admin", "#E9B8F2", lines=[
        "inherits User", "displayMenu()"
    ])
    draw_box(d, 240, 330, 170, 92, "Customer", "#E9B8F2", lines=[
        "cart", "history", "orderHistory", "getActivityScore()"
    ])
    draw_box(d, 470, 345, 155, 86, "ShoppingCart", "#FDE68A", lines=[
        "items", "addProduct()", "removeProduct()",
        "updateQuantity()", "calculateTotal()"
    ])
    draw_box(d, 650, 350, 90, 78, "CartItem", "#FDE68A", lines=[
        "product", "quantity", "getSubtotal()"
    ])

    draw_box(d, 40, 205, 130, 80, "Product", "#2DD4BF", lines=[
        "id, name, price", "stock", "viewCount", "purchaseCount"
    ])
    draw_box(d, 240, 225, 130, 60, "Category", "#C7D2FE", lines=[
        "id", "name"
    ])
    draw_box(d, 240, 135, 170, 78, "InteractionHistory", "#C7D2FE", lines=[
        "viewedProducts", "purchasedProducts",
        "addView()", "addPurchase()"
    ])

    draw_box(d, 470, 215, 140, 72, "Order", "#FDE68A", lines=[
        "orderId", "customerId", "items", "totalAmount"
    ])
    draw_box(d, 650, 215, 90, 72, "OrderItem", "#FDE68A", lines=[
        "product", "quantity", "unitPrice"
    ])

    draw_box(d, 40, 40, 165, 92, "ECommerceSystem", "#2DD4BF", lines=[
        "categories, products", "customers, admins, orders",
        "run()", "seedData()", "checkout()"
    ])
    draw_box(d, 270, 38, 160, 88, "RecommendationEngine", "#FCA5A5", lines=[
        "calculateScore()", "getCategoryBonus()",
        "recommendProducts()"
    ])
    draw_box(d, 470, 38, 150, 88, "StatisticsManager", "#FCA5A5", lines=[
        "getMostViewedProducts()", "getBestSellingProducts()",
        "getMostActiveUsers()", "calculateRevenue()"
    ])
    draw_box(d, 650, 40, 90, 82, "FileManager", "#FCA5A5", lines=[
        "save*", "load*"
    ])

    connect(d, 160, 390, 240, 450, "inherits")
    connect(d, 160, 390, 240, 360, "inherits")
    connect(d, 410, 375, 470, 385, "has-a")
    connect(d, 625, 387, 650, 389, "contains")
    connect(d, 170, 245, 240, 255, "belongs to")
    connect(d, 410, 175, 240, 175, "has-a")
    connect(d, 540, 250, 650, 250, "contains")
    connect(d, 160, 85, 240, 85, "uses")
    connect(d, 205, 85, 470, 85, "uses")
    connect(d, 205, 78, 650, 78, "uses")
    connect(d, 140, 132, 100, 205, "manages")
    connect(d, 205, 120, 325, 330, "manages")
    connect(d, 205, 110, 470, 235, "manages")

    return d


def class_card(drawing, x, y, w, h, name, attrs, methods, fill):
    drawing.add(
        Rect(
            x, y, w, h,
            rx=10, ry=10,
            fillColor=colors.HexColor(fill),
            strokeColor=colors.HexColor("#12344D"),
            strokeWidth=1.2,
        )
    )
    drawing.add(
        Line(x, y + h - 24, x + w, y + h - 24, strokeColor=colors.HexColor("#12344D"), strokeWidth=1.0)
    )
    drawing.add(
        Line(x, y + h - 78, x + w, y + h - 78, strokeColor=colors.HexColor("#12344D"), strokeWidth=1.0)
    )
    drawing.add(
        String(x + 8, y + h - 16, name, fontName=FONT_NAME, fontSize=11, fillColor=colors.HexColor("#12344D"))
    )

    attr_y = y + h - 38
    for line in attrs[:4]:
        drawing.add(String(x + 8, attr_y, line, fontName=FONT_NAME, fontSize=7.8, fillColor=colors.black))
        attr_y -= 10

    method_y = y + h - 92
    for line in methods[:6]:
        drawing.add(String(x + 8, method_y, line, fontName=FONT_NAME, fontSize=7.6, fillColor=colors.black))
        method_y -= 9


def class_overview_page_one():
    d = Drawing(760, 500)
    class_card(d, 20, 300, 170, 170, "User", [
        "- id: int", "- username: string", "- role: string"
    ], [
        "+ User()", "+ User(id, username, role)",
        "+ getId()", "+ getUsername()", "+ getRole()",
        "+ displayMenu()"
    ], "#E6FFFA")
    class_card(d, 210, 300, 170, 170, "Customer", [
        "- cart: ShoppingCart", "- history: InteractionHistory", "- orderHistory: vector<int>"
    ], [
        "+ getCart()", "+ getInteractionHistory()",
        "+ getOrderHistory()", "+ addOrderId()", "+ getActivityScore()",
        "+ displayMenu()"
    ], "#F3E8FF")
    class_card(d, 400, 300, 150, 170, "Admin", [
        "- inherits User"
    ], [
        "+ Admin()", "+ Admin(id, username)", "+ displayMenu()"
    ], "#F3E8FF")
    class_card(d, 570, 300, 170, 170, "Category", [
        "- id: int", "- name: string"
    ], [
        "+ Category()", "+ Category(id, name)",
        "+ getId()", "+ getName()", "+ setId()", "+ setName()"
    ], "#DBEAFE")

    class_card(d, 20, 90, 240, 180, "Product", [
        "- id, name", "- category: Category",
        "- compatibleDevices: vector<string>", "- price, stock, viewCount, purchaseCount"
    ], [
        "+ Product()", "+ getters/setters",
        "+ increaseViewCount()", "+ increasePurchaseCount()",
        "+ decreaseStock()", "+ display()"
    ], "#E6FFFA")
    class_card(d, 280, 90, 210, 180, "InteractionHistory", [
        "- viewedProducts: map<int,int>", "- purchasedProducts: map<int,int>"
    ], [
        "+ addView()", "+ addPurchase()", "+ getViewCount()",
        "+ getPurchaseCount()", "+ getTotalViews()", "+ clear()"
    ], "#DBEAFE")
    class_card(d, 510, 90, 230, 180, "ECommerceSystem", [
        "- categories, products", "- customers, admins, orders",
        "- recommendationEngine", "- statisticsManager, fileManager"
    ], [
        "+ run()", "+ seedData()", "+ customerMenu()",
        "+ adminMenu()", "+ browseProducts()", "+ checkout()"
    ], "#E6FFFA")
    return d


def class_overview_page_two():
    d = Drawing(760, 500)
    class_card(d, 20, 285, 180, 185, "CartItem", [
        "- product: Product", "- quantity: int"
    ], [
        "+ CartItem()", "+ CartItem(product, quantity)",
        "+ getProduct()", "+ getQuantity()", "+ getSubtotal()",
        "+ setQuantity()"
    ], "#FEF3C7")
    class_card(d, 220, 285, 230, 185, "ShoppingCart", [
        "- items: vector<CartItem>"
    ], [
        "+ addProduct()", "+ removeProduct()", "+ updateQuantity()",
        "+ calculateTotal()", "+ getItemCount()", "+ displayCart()"
    ], "#FEF3C7")
    class_card(d, 470, 285, 180, 185, "OrderItem", [
        "- product: Product", "- quantity: int", "- unitPrice: double"
    ], [
        "+ OrderItem()", "+ getProduct()", "+ getQuantity()",
        "+ getUnitPrice()", "+ getSubtotal()", "+ display()"
    ], "#FEF3C7")
    class_card(d, 20, 70, 200, 185, "Order", [
        "- orderId: int", "- customerId: int", "- items: vector<OrderItem>", "- totalAmount: double"
    ], [
        "+ addItem()", "+ calculateTotal()", "+ getOrderId()",
        "+ getCustomerId()", "+ getItems()", "+ display()"
    ], "#FEF3C7")
    class_card(d, 240, 70, 230, 185, "RecommendationEngine", [
        "- stateless service"
    ], [
        "+ calculateScore()", "+ getCategoryBonus()",
        "+ recommendProducts(customer, products, topN)"
    ], "#FECACA")
    class_card(d, 490, 70, 230, 185, "StatisticsManager", [
        "- stateless service"
    ], [
        "+ getMostViewedProducts()", "+ getBestSellingProducts()",
        "+ getMostActiveUsers()", "+ calculateRevenue()"
    ], "#FECACA")
    class_card(d, 670, 285, 70, 185, "File\nManager", [
        "- stateless service"
    ], [
        "+ saveCategories()", "+ loadCategories()",
        "+ saveProducts()", "+ loadProducts()", "+ saveOrders()", "+ loadOrders()"
    ], "#FECACA")
    return d


def build_pdf():
    doc = SimpleDocTemplate(
        str(OUTPUT),
        pagesize=landscape(A4),
        leftMargin=1.2 * cm,
        rightMargin=1.2 * cm,
        topMargin=1.2 * cm,
        bottomMargin=1.2 * cm,
        title="Project 2 Class Mind Map",
        author="OpenAI Codex",
    )

    story = [
        Paragraph("Project 2 Class Mind Map", STYLES["TitleMind"]),
        Paragraph(
            "Sơ đồ tổng quan quan hệ giữa các class, cùng thuộc tính và phương thức chính theo code hiện tại.",
            STYLES["SubMind"],
        ),
        Spacer(1, 0.2 * cm),
    ]

    story.append(relationship_map())
    story.append(PageBreak())
    story.append(Paragraph("Class Details - Core Models", STYLES["HeadMind"]))
    story.append(Paragraph(
        "Trang này tập trung vào các class lõi: User, Customer, Admin, Category, Product, InteractionHistory và ECommerceSystem.",
        STYLES["BodyMind"],
    ))
    story.append(class_overview_page_one())
    story.append(PageBreak())
    story.append(Paragraph("Class Details - Business And Services", STYLES["HeadMind"]))
    story.append(Paragraph(
        "Trang này mô tả các class cho cart, order và các service xử lý recommendation, statistics, file I/O.",
        STYLES["BodyMind"],
    ))
    story.append(class_overview_page_two())

    doc.build(story, onFirstPage=add_page_number, onLaterPages=add_page_number)


if __name__ == "__main__":
    build_pdf()
    print(f"Created PDF: {OUTPUT}")
